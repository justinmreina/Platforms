import cgi
import xml.etree.ElementTree as ET
from xml.etree.ElementTree import Element, SubElement
import os
from collections import namedtuple
from os import path
import urllib
import re
import xml.dom.minidom
import errno
import libSearch
import win32api

cloud = True

def mkdir_p(p):
    try:
        os.makedirs(p)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(p):
            pass
        else: raise

Define = namedtuple('Define', ['expression'])
CompilerOption = namedtuple('CompilerOption', ['option'])
Include = namedtuple('Include', ['path'])
Library = namedtuple('Library', ['path'])

#class File(namedtuple('File', ['source', 'target', 'action', 'virtual', 'openOnCreate', 'exclude'])):
#    def __new__(cls, source, target, action='link', virtual='true', openOnCreate='false', exclude='false'):
#        return super(File, cls).__new__(cls, source, target, action, virtual, openOnCreate, exclude)

class File:
    def __init__(self, source, target, action='link', virtual='true', openOnCreate='false', exclude='false'):
        self.source = source
        self.target = target
        self.action = action
        self.virtual = virtual
        self.openOnCreate = openOnCreate
        self.exclude = exclude
    def __repr__(self):
        return "File(%s: %s)" % (self.source, self.target)

class PathVar:
    def __init__(self, name, path, scope='project'):
        self.name = name
        self.path = path
        self.scope = scope
    def __repr__(self):
        return "PathVar(%s: %s)" % (self.name, self.path)


CProject = namedtuple('CProject', ['path', 'istirtos', 'defines', 'includes', 'libs', 'linkeroptions', 'compileroptions'])
Project = namedtuple('Project', ['path', 'name', 'pathvars', 'files'])
CCSProject = namedtuple('CCSProject', ['relpath', 'lastpath', 'debuggerConnection', 'ccsproject', 'cproject', 'project'])

os.chdir(os.path.dirname(os.path.realpath(__file__)))



################
# Get information from the xml files

def extract_xml(root, xpath, operation):
    return filter(None, map(operation, root.findall(xpath)))

xpath_defines  =  './storageModule/cconfiguration/storageModule/configuration[@name="{buildConfig}"]/folderInfo/toolChain/tool[@name="ARM Compiler"]/option[@valueType="definedSymbols"]/listOptionValue'
xpath_linkerdefines = './storageModule/cconfiguration/storageModule/configuration[@name="{buildConfig}"]/folderInfo/toolChain/tool[@name="ARM Linker"]/option[@valueType="definedSymbols"]/listOptionValue'
xpath_includes =  './storageModule/cconfiguration/storageModule/configuration[@name="{buildConfig}"]/folderInfo/toolChain/tool/option[@valueType="includePath"]/listOptionValue'
xpath_libs     =  './storageModule/cconfiguration/storageModule/configuration[@name="{buildConfig}"]/folderInfo/toolChain/tool/option[@valueType="libs"]/listOptionValue'
xpath_builddefs = './storageModule/cconfiguration/storageModule/configuration[@name="{buildConfig}"]/folderInfo/toolChain/option/listOptionValue'
xpath_excludes =  './storageModule/cconfiguration/storageModule/configuration[@name="{buildConfig}"]/sourceEntries/entry'

xpath_pathVars = './variableList/variable'
xpath_filelinks = './linkedResources/link'



def get_attrib_value(n):
    return n.attrib['value']

def get_pathvar(n):
    return PathVar(n.findtext('./name'), n.findtext('./value'), 'project')

def get_filelink(n):
    if int(n.findtext('type')) == 1:
        return File(n.findtext('./locationURI'), n.findtext('./name'))
    else:
        return None

def find_stack_libraries(optfile):
    ret = []
    argparams = '../Projects/tools/LibSearch/parameters.xml'
    libdir = '../Projects/ble/Libraries/CC26xx/IAR/'
    argconfig = 'FlashROM'

    libs = libSearch.libsToSearch(argparams)
    for lib in libs:
        # Parse template file to determine config parameters to match against
        (relPath, nameExt, addConfig, params) = libSearch.parseXML(lib, argparams)
        p = os.path.join(libdir,relPath)
        config = ""
        if addConfig and addConfig != "0":
            config += argconfig

        # Parse given opt file to determine state of parameters given in XML
        optparams = libSearch.parseOPT(optfile, params)

        # Search through all possible libraries to find lib that matches parameters in opt
        lib = libSearch.matchLib(lib,p,optparams,config)

        if lib:
            ret.append(lib)

    return ret

############
# Enumerate the files. Should also fill in namedtuples here to avoid all the replacing.

def relativize(relativeTo, orig, pvs, stepsdown=1, buildVar=False):
    orig = os.path.abspath(orig) if os.path.exists(os.path.abspath(orig)) else orig
    for i in xrange(stepsdown):
        tryrelative = path.normpath(pvs.get(relativeTo, 'UNDEFINED') + (path.sep+'..'+path.sep) * i)
        common = path.commonprefix([orig.lower(), tryrelative.lower()])
        if tryrelative.lower() in common.lower():
            if buildVar: relativeTo = "${"+relativeTo+"}"
            return orig.replace(tryrelative, relativeTo + (path.sep+'..') * i)
    return orig

def strip_path(p):
    p = urllib.unquote(p)
    p = p.replace('"', '')
    p = p.replace('${', '')
    p = p.replace('}', '')
    p = p.replace('file:/', '')
    p = p.replace(path.altsep, path.sep)
    return p

def fix_parent(p):
    m = re.match(".*(PARENT-(\d)-([A-Z0-9a-z_]*)).*", p)
    if m is not None:
        alias, numdown, pathvar = m.groups()
        p = p.replace(alias, pathvar + (path.sep+'..'+path.sep) * int(numdown))
    return p

def resolve_path(pvs, _path):
    for k, v in pvs.items():
        _path = _path.replace(k, v)
    return _path

def fix_path_all(s, pvs, buildVar=False):
    s = strip_path(s)
    s = fix_parent(s)
    s = resolve_path(pvs, s)
    s = path.normpath(s)
    try:
        s = win32api.GetLongPathName(win32api.GetShortPathName(s))
    except Exception as e:
        print "Couldn't get full path via win32api:", s, e
    s = relativize('PROJECT_LOC', s, pvs, 3, buildVar)
    s = relativize('TI_BLE_SDK_BASE', s, pvs, 1, buildVar)
    s = relativize('TI_RTOS_DRIVERS_BASE', s, pvs, 1, buildVar)
    s = relativize('CC26XXWARE', s, pvs, 1, buildVar)
    s = s.replace('\\', '/')
    return s

excludes = None
def findprojects(arg, dirname, names):
    global excludes
    if '.ccsproject' in names:
        lastpath = path.split(dirname)[-1]
        ccsprojectpath = path.join(dirname, '.ccsproject')
        cprojectpath = path.join(dirname, '.cproject')
        projectpath = path.join(dirname, '.project')


        ###################
        #### .project #####
        root = ET.parse(projectpath).getroot()
        # Name
        name = root.find('./name').text
        # Path variables
        pathvars = extract_xml(root, xpath_pathVars, get_pathvar)
        pathvars.extend([PathVar('TI_BLE_SDK_BASE', os.path.normpath(os.path.join(path.dirname(path.abspath(__file__)), '../')), 'project')])
        pathvars.extend([PathVar('PROJECT_LOC', dirname, 'project')])
        #pathvars.extend([PathVar('PROJECT_IMPORT_LOC', './', 'project')])

        # Dictionary of pathvars
        pvs = dict([(pv.name, pv.path) for pv in pathvars])
        # Try to resolve absolute. Do two passes.
        for i in xrange(2):
            for k, v in pvs.items():
                v = strip_path(v)
                v = fix_parent(v)
                v = resolve_path(pvs, v)
                if path.isdir(path.realpath(v)):
                    pvs[k] = path.normpath(path.realpath(v))
                else:
                    pvs[k] = v

        # Replace pathvars with nice ones. Remove CCS magic ones and unneeded
        toremove = ['PROJECT_LOC', 'ORG_PROJ_DIR', 'CG_TOOL_ROOT']
        pathvars = [PathVar(k, v) for k, v in pvs.items() if not k in toremove]

        # Ignored imports
        ignoredIncludes = ['CG_TOOL_ROOT']

        # File links
        links = extract_xml(root, xpath_filelinks, get_filelink)
        for l in links:
            l.source = fix_path_all(l.source, pvs)
            ### USE MAGIC PROJECT_IMPORT_LOC for local files that are linked ###
            l.source = l.source.replace('PROJECT_LOC', 'PROJECT_IMPORT_LOC')
            l.target = os.path.dirname(l.target)
            #print l.source

        #######################
        ###### .cproject ######
        root = ET.parse(cprojectpath).getroot()
        # Figure out which BuildConfig to use
        # If FlashROM exists, use that.
        thisBuildConfig = 'FlashROM'
        if not len(root.findall('./storageModule/cconfiguration/storageModule/configuration[@name="FlashROM"]')):
            thisBuildConfig = 'FlashOnlyOAD'

        # defines
        defines = extract_xml(root, xpath_defines.format(buildConfig=thisBuildConfig), get_attrib_value)
        linkerdefines = extract_xml(root, xpath_linkerdefines.format(buildConfig=thisBuildConfig), get_attrib_value)

       # includes
        includes = extract_xml(root, xpath_includes.format(buildConfig=thisBuildConfig), get_attrib_value)
        _incfixed = []
        for inc in [_i for _i in includes if not any([ign in _i for ign in ignoredIncludes])]:
            _inc = fix_path_all(inc, pvs, True)
            ### USE MAGIC PROJECT_IMPORT_LOC for local files that are linked ###
            _inc = _inc.replace('PROJECT_LOC', 'PROJECT_IMPORT_LOC')
            _incfixed.append(_inc)
            #print inc
        #includes = extraIncludes.values()
        includes = []
        includes.extend(_incfixed)

        builddefs = extract_xml(root, xpath_builddefs.format(buildConfig=thisBuildConfig), get_attrib_value)
        isTirtos = False
        if any(['RTSC' in k for k in builddefs]):
            isTirtos = True

        # libraries
        libraries = [fix_path_all(lib, pvs, True) for lib in extract_xml(root, xpath_libs.format(buildConfig=thisBuildConfig), get_attrib_value)]

        # Switch out stack libraries with fixed link
        optfile = [k.source for k in links if 'buildConfig.opt' in k.source]
        stacklibs = []
        if len(optfile):
            optfile = optfile[0]
            optfile = strip_path(optfile)
            optfile = optfile.replace('PROJECT_IMPORT_LOC', 'PROJECT_LOC')
            optfile = resolve_path(pvs, optfile)
            foundlibs = find_stack_libraries(optfile)
            for l in foundlibs:
                l = fix_path_all(l, pvs, True)
                stacklibs.append(l)

            # Filter out existing library includes
            libraries = [l for l in libraries if not 'CC2640Stack' in l and not 'CC2650Stack' in l]
            libraries.extend(stacklibs)
            # Switch out libraries shown in file links
            links = [l for l in links if not 'CC2640Stack' in l.source and not 'CC2650Stack' in l.source]
            stacklibs = [fix_path_all(l, pvs, False) for l in stacklibs] # Change to file format instead of buildvar
            links.extend([File(l, 'LIB') for l in stacklibs])


        # Find excluded files, and modify the file links
        excludes = root.find(xpath_excludes.format(buildConfig=thisBuildConfig))
        excludes = [] if excludes is None else excludes.attrib['excluding'].split('|')
        for l in links[:]:
            targetFile = l.target + '/' + l.source.split('/')[-1]
            if any([targetFile.startswith(e) for e in excludes]):
                l.exclude = 'true'
                # Remove it as well. Can't have more build configs in projectspecs anyway.
                links.remove(l)

        linkeroptions = ['--define='+d for d in linkerdefines]
        # Find info from ccsLinkerDefines.cmd, workaround for cloud, re: problem with setting LINK-ORDER from projectspec
        if cloud:
            linkerdefinefile = [k.source for k in links if 'ccsLinkerDefines.cmd' in k.source]
            if len(linkerdefinefile):
                linkerdefinefile = linkerdefinefile[0]
                linkerdefinefile = linkerdefinefile.replace('PROJECT_IMPORT_LOC', 'PROJECT_LOC')
                linkerdefinefile = resolve_path(pvs, linkerdefinefile)
                with open(linkerdefinefile, 'r') as f:
                    for line in f.readlines():
                        if '--define' in line:
                            linkeroptions.append(line.strip())                     # Add to linker defines
                            defines.append(line.strip().replace('--define=', ''))  # Add to compiler defines
            # Remove linker command s with defines from file import list
            links = [l for l in links if not 'ccsLinkerDefines.cmd' in l.source and not 'ccsCompilerDefines.bcfg' in l.source]

            # Also add APP_BASE=0x0000 for all, until
        compileroptions = []
        # Add buildConfig.opt to compiler options as a cmd file
        if cloud:
            buildconfigfile = [k.source for k in links if 'buildConfig.opt' in k.source]
            if len(buildconfigfile):
                buildconfigfile = buildconfigfile[0]
                buildconfigfile = buildconfigfile.replace('PROJECT_IMPORT_LOC', 'PROJECT_LOC')
                #buildconfigfile = resolve_path(pvs, buildconfigfile)
                #buildconfigfile = relativize('PROJECT_LOC', buildconfigfile, pvs, 3, True)
                buildconfigfile = fix_path_all(buildconfigfile, pvs, True)
                buildconfigfile = buildconfigfile.replace('PROJECT_LOC', 'PROJECT_IMPORT_LOC')
                compileroptions.append('--cmd_file=' + buildconfigfile + '')


        ##.ccsproject file
        root = ET.parse(ccsprojectpath).getroot()
        debuggerConnection = extract_xml(root, 'connection', get_attrib_value)[0].split('/')[-1]


        # Change some of the files to be copied into the workspace
        extraIncludes = dict()
        copySources = ['PROJECT_IMPORT_LOC/', 'bleUserConfig']
        copyTargets = ['PROFILES', 'Board', 'Startup']

        if not name.endswith('Stack'):
            for l in links[:]:
                if ( any([s in l.source for s in copySources]) or any([t in l.target for t in copyTargets])) and (l.source.endswith('.c') or l.source.endswith('.h')):
                    # Get path relative to .projectspec file
                    l.source = l.source.replace('PROJECT_IMPORT_LOC', 'PROJECT_LOC') # Switch back to real pathVar
                    abspath = resolve_path(pvs, l.source)
                    relpath = relativize('PROJECT_LOC', abspath, pvs, 10)
                    relpath = relpath.replace('\\', '/')
                    relpath = relpath.replace('PROJECT_LOC/', '') # Relative directory is PROJECT_LOC, so replace.
                    if 'SRF06EB' in relpath:
                        relpath = relpath.split('SRF06EB/')
                        relpath = ''.join([relpath[0], 'SRF06EB/CC2650EM_7ID/', relpath[1]])
                    elif 'Board_patch/Board.c' in relpath:
                        relpath = relpath.split('Board_patch/')
                        relpath = ''.join([relpath[0], 'Board_patch/CC26XXST_0120/', relpath[1]])
                    l.source = relpath #l.source.replace(localSourcePath, '') # Change to relative path otherwise can't copy
                    l.action = 'copy'
                    # Add Board.h as well
                    if 'Board.c' in l.source and not any(['Board.h' in x.source for x in links]):
                        links.append(File(l.source.replace('Board.c', 'Board.h'), l.target, l.action))

                    extraIncludes[l.target] = "${PROJECT_LOC}/"+l.target

            # Add extra includes. Must be sorted by generator so they win.
            _inc = includes
            includes = extraIncludes.values()
            includes.extend(_inc)

        # Work around the fact that the 06EB is default in the SDK.
        if cloud:
            if "SensorTag" in name:
                debuggerConnection = "TIXDS110_Connection.xml"


        print lastpath
        arg[lastpath] = CCSProject(dirname, lastpath,  debuggerConnection, None, CProject(cprojectpath, isTirtos, defines, includes, libraries, linkeroptions, compileroptions), Project(projectpath, name, pathvars, links))

ccsProjects = dict()
os.path.walk("../", findprojects, ccsProjects)


### Try to find commonalities
class Node:
    def __init__(self, name, alias=None, children=None):
        self.name = name
        self.alias = name if alias is None else alias
        self.children = children if children is not None else []
        self.ccsproject = None
        self.fileset = set()
        self.includeset = set()


nodes =\
    Node("All projects", "all", [
        Node("Applications", "app", [
            Node("Peripheral Applications", "peripheral", [
                Node("CC2650DK", "cc2650dk", [
                    Node('GlucoseSensor'),
                    Node('HIDEmuKbd'),
                    Node('KeyFob'),
                    #
                    Node('SimpleBLEPeripheral'),
                    ]),
                Node("CC2650ST", "cc2650st", [
                    Node('SensorTag'),
                    ])
                ]),
            Node("Central Applications", "central", [
                Node("CC2650DK", "cc2650dk", [
                    Node('GlucoseCollector'),
                    Node('SimpleBLECentral'),
                    ])
                ]),
            Node("GAP_All Apps", "gapAll", [
                Node("CC2650DK", "cc2650dk", [
                    Node("HostTest"),
                ]),
            ]),
            Node("Non-connectable Applications", "nonconn", [
                Node("CC2650DK", "cc2650dk", [
                    Node('SimpleBLEBroadcaster'),
                    Node('SimpleBLEObserver')
                    ])
                ]),
            ]),
        Node("Stacks", "stack", [
            Node("Peripheral Stacks", "peripheral", [
                Node("CC2650DK", "cc2650dk", [
                    Node('GlucoseSensorStack'),
                    Node('HIDEmuKbdStack'),
                    Node('KeyFobStack'),
                    #
                    Node('SimpleBLEPeripheralStack'),
                    ]),
                Node("CC2650ST", "cc2650st", [
                    Node('SensorTagStack'),
                    ])
                ]),
            Node("Central Stacks", "central", [
                Node("CC2650DK", "cc2650dk", [
                    Node('GlucoseCollectorStack'),
                    Node('SimpleBLECentralStack'),
                ])]),
            Node("GAP_All Stacks", "gapAll", [
                Node("CC2650DK", "cc2650dk", [
                    Node("HostTestStack"),
                ]),
            ]),
            Node("Non-connectable Stacks", "nonconn", [
                Node("CC2650DK", "cc2650dk", [
                    Node('SimpleBLEBroadcasterStack'),
                    Node('SimpleBLEObserverStack')
                ])]),
            ])
        ])

Xnodes = Node("HostTest", 'all', [
    Node("HostTestApp"),
    Node("HostTestStack"),
])

def all_descendants(node):
    children = []
    children += node.children # Without [] and += children just points to node.children, which has side-effects
    for ch in children:
        children += all_descendants(ch)
    return children

def all_ancestors(node):
    parents = []
    parents += node.parent
    parents += all_ancestors(node.parent)
    return parents

def recursive_set(nodes, which):
    return set.intersection(*[ch.get_set(which) for ch in nodes])

whats = [('file', 'project','(x.source, x.target, x.action, x.exclude)'), ('include', 'cproject', 'x'), ('define', 'cproject', 'x'), ('lib', 'cproject', 'x')]

wheres = [('file', ('source', 'target', 'action', 'exclude')), ('include', 'include'), ('define', 'define'), ('lib', 'lib')]

def populate_tree2(node):
    for ch in node.children:
        populate_tree2(ch)  # Fill out leaves first

    if not node.children: # Leaves have the files
        node.ccsproject = ccsProjects[node.name]
        for what, where, how in whats:
            exec('node.'+what+'set = set(['+how+' for x in node.ccsproject.'+where+'.'+what+'s])') #node.fileset = set([(l.source, l.target) for l in node.ccsproject.project.links])
    else:
        for what, where, how in whats:
            exec('node.'+what+'set = set.intersection(*[ch.'+what+'set for ch in all_descendants(node)])') #children_common_includes = set.intersection(*[ch.includeset for ch in all_descendants(node)])

populate_tree2(nodes)


def climb_tree(node, level=0):
    length = len(set.union(*[k.fileset for k in all_descendants(node) if k.fileset])) if node.children else len(node.fileset)
    print "  " * level, node.name, length
    for ch in node.children:
        #length += climb_tree(ch, level+1)
        climb_tree(ch, level+1)
    return length

print
print "Summary of included files"
climb_tree(nodes)




def unique_tree(node, level=0, parent=None):
    #whats = ['file', 'include']

    for what, where, how in whats:
        exec('children_common_'+what+'s = set.intersection(*[ch.'+what+'set for ch in all_descendants(node)]) if node.children else None') #    children_common_files = set.intersection(*[ch.fileset for ch in all_descendants(node)]) if node.children else None
        exec('sibling_common_'+what+'s = set.intersection(*[ch.'+what+'set for ch in parent.children]) if parent else None')               #    sibling_common_files = set.intersection(*[ch.fileset for ch in parent.children]) if parent else None

    if not parent:
        for what, where, how in whats:
            exec('node.'+what+'set_unique = children_common_'+what+'s') #node.fileset_unique = children_common_files
    else:
        if len(node.children):
            for what, where, how in whats:
                exec('node.'+what+'set_unique = children_common_'+what+'s - sibling_common_'+what+'s') #node.fileset_unique = children_common_files - sibling_common_files
        else:
            for what, where, how in whats:
                exec('node.'+what+'set_unique = node.'+what+'set - sibling_common_'+what+'s') #node.fileset_unique = node.fileset - sibling_common_files

    for ch in node.children:
        unique_tree(ch, level+2, node)

print
print
unique_tree(nodes)

def print_tree(node, what, level=0, maxlen=0):
    if what == 'file': _what = 'fileset_unique'
    elif what == 'include': _what = 'includeset_unique'
    elif what == 'define': _what = 'defineset_unique'
    else: return

    maxfmt = ""
    if not maxlen:
        if len(eval('node.'+_what)):
            maxlen = max([len(u) if type(u) is not type(()) else len(u[0]) for u in eval('node.'+_what)])
            maxfmt = eval("'{:<"+str(maxlen)+"}'")

    if not level:
        print "  " * level, "Common to",node.name,'-', eval('len(node.'+_what+')'), what
    else:
        print
        print "  " * level, "#", "Unique to", node.name,'-', eval('len(node.'+_what+')'), what

    for u in sorted(eval('node.'+_what)):
        if type(u) is type([]) or type(u) is type(()):
            print "  " * (level+1) + '   --> '.join([maxfmt.format(k) if not i else k for i, k in enumerate(u)])
        else:
            print "  " * (level+1), u

    for ch in node.children:
        print_tree(ch, what, level+2)

#print_tree(nodes, 'file')
print
print " ##############################################"
print
#print_tree(nodes, 'include')
print
print " ##############################################"
print
#print_tree(nodes, 'define')


##def make_generator_xml(node, out=None, outpath="./out/", parents=''):
##    # This node
##    parents += node.alias + '_'
##
##    n = Element('files')
##    for s, t in sorted(node.fileset_unique):
##        k = SubElement(n, 'file')
##        x = SubElement(k, 'source').text = s
##        x = SubElement(k, 'target').text = t
##
##    filename = str.lower(parents + 'files.xml')
##    prettyxml = xml.dom.minidom.parseString(ET.tostring(n)).toprettyxml()
##    print "####", filename
##    #print prettyxml
##
##    f = open(outpath + filename, 'w')
##    f.write(prettyxml)
##    f.close()
##
##    # Then descend
##    for ch in node.children:
##        make_generator_xml(ch, out, outpath, parents)


def make_generator_xml2(node, out=None, outpath="./out/", parents=''):
    # This node
    if len(parents):
        parents += '_' + node.alias
    else:
        parents = node.alias

    # Make projectspec template if leaf
    if not len(node.children):
        relPathToCommon = os.path.relpath('.\\out\\', start=node.ccsproject.relpath) + '\\'

        p = Element('project')
        SubElement(p, 'name').text = node.alias
        SubElement(p, 'isTIRTOS').text = str.lower(str(node.ccsproject.cproject.istirtos))
        SubElement(p, 'compileroptions', attrib={'include': relPathToCommon + 'compileroptions_'+'_'.join(parents.split('_')[1:2])+'_cloud.xml'}) # app, stack
        if node.alias == 'HostTestApp': # Special case for HostTest because it needs buildComponents.
            SubElement(p, 'compileroptions', attrib={'include': relPathToCommon + 'compileroptions_hosttestapp_cloud.xml'}) # app, stack
        SubElement(p, 'pathvars', attrib={'include': relPathToCommon + 'pathvars.xml'})
        SubElement(p, 'linkeroptions', attrib={'include': relPathToCommon + 'linkeroptions_'+'_'.join(parents.split('_')[1:2])+'.xml'}) # app, stack
        SubElement(p, 'attributes', attrib={'include': relPathToCommon + 'attributes_'+'_'.join(parents.split('_')[1:2])+'.xml'}) # app, stack
        SubElement(p, 'debuggerConnection').text = node.ccsproject.debuggerConnection

        if len(node.ccsproject.cproject.linkeroptions):
            r = SubElement(p, 'linkeroptions')
            for o in node.ccsproject.cproject.linkeroptions:
                SubElement(r, 'linkeroption').text = o

        if len(node.ccsproject.cproject.compileroptions):
            r = SubElement(p, 'compileroptions')
            for o in node.ccsproject.cproject.compileroptions:
                SubElement(r, 'compileroption').text = o

        # Add project reference to stack project. Assume it's similarly named, but ends with Stack
        if not node.alias.endswith('Stack'):
            r = SubElement(p, 'projectrefs')
            refName = node.alias + 'Stack'
            if node.alias == 'HostTestApp':
                refName = 'HostTestStack'
            SubElement(r, 'ref', attrib={'boundary': 'true'}).text = refName

        # Add reference to common dependencies
        for item, sources in wheres:
            dependencies = parents.split('_')[:-1]
            for i, d in enumerate(dependencies):
                d = '_'.join(dependencies[:i+1])
                SubElement(p, item+'s', attrib={'include': relPathToCommon + str(item + '_' + d +'.xml'), 'level': str(i)})

    # Iterate over all include types
    for item, sources in wheres:
        if len(node.children):
            n = Element(item+'s')
        else:
            n = SubElement(p, item+'s') # Add to existing project

        for k in sorted(eval('node.'+item+'set_unique')):
            z = SubElement(n, item)
            if type(sources) is type(()):
                for i, v in enumerate(sources):
                    #print len(sources), k
                    SubElement(z, sources[i]).text = k[i]
            else:
                z.text = k

        if len(node.children): # Output support file if it's not a leaf node
            filename = str.lower(item+ '_' + parents + '.xml')
            prettyxml = xml.dom.minidom.parseString(ET.tostring(n)).toprettyxml()
            with open(outpath + filename, 'w') as f:
                print "####", filename
                f.write(prettyxml)

    if not len(node.children):
        filename = node.alias +'.xml'
        prettyxml = xml.dom.minidom.parseString(ET.tostring(p)).toprettyxml()
        print "####", filename
        pname = '/'.join(path.split(node.ccsproject.relpath)[:-1]) + '/Cloud'
        mkdir_p(pname)
        with open(pname +'\\' + filename, 'w') as f:
            f.write(prettyxml)

    # Then descend
    for ch in node.children:
        make_generator_xml2(ch, out, outpath, parents)



make_generator_xml2(nodes)
