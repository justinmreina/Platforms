import jinja2
from jinja2 import Environment, PackageLoader, FileSystemLoader, TemplateNotFound, Template
import xml.etree.ElementTree as ET
import sys
import os, errno
import time
from collections import namedtuple
from collections import OrderedDict
import shutil
import cgi

def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise

cloud = True

## Template content types
Define = namedtuple('Define', ['expression'])

CompilerOption = namedtuple('CompilerOption', ['option'])

Include = namedtuple('Include', ['path'])

Library = namedtuple('Library', ['path'])

class File(namedtuple('File', ['source', 'target', 'action', 'virtual', 'openOnCreate', 'exclude'])):
    def __new__(cls, source, target, action='link', virtual='true', openOnCreate='false', exclude='false'):
        return super(File, cls).__new__(cls, source, target, action, virtual, openOnCreate, exclude)

PathVar = namedtuple('PathVar', ['name', 'path', 'scope'])

#projectfilepaths = ["../ble_2_0_rtm/CC26xx/CCS/MyPeripheralApp/ble_peripheral.project.xml", "../ble_2_0_rtm/CC26xx/CCS/MyPeripheralStack/ble_peripheral_stack.project.xml"]

GenProject = namedtuple('GenProject', ['name', 'sourcepath', 'recipes', 'templates', 'outpath'])

# Read from instruction file
os.chdir(os.path.dirname(os.path.realpath(__file__)))
genroot = ET.parse('generate.xml').getroot()

genprojects = []

for genproject in genroot.findall('./project'):
    name = genproject.find('./name').text if genproject.find('./name') is not None else None
    sourcepath = os.path.realpath(genproject.find('./sourcepath').text) if genproject.find('./sourcepath') is not None else None
    recipes = [k.text for k in genproject.findall('./recipes/recipe')]
    templates = [k.text for k in genproject.findall('./templates/template')]
    outpath = os.path.realpath(genproject.find('./outpath').text) if genproject.find('./outpath') is not None else None # Find absolute here, while in modules.xml's folder.
    genprojects.append(GenProject(name, sourcepath, recipes, templates, outpath))


for genproject in genprojects:
    # Record used xml files so they won't be copied
    genprojectxmls = []
    for recipe in genproject.recipes:
        # Clean context
        context = dict()

        # Construct environment for template rendering
        os.chdir(os.path.dirname(os.path.realpath(__file__)))
        env = Environment(loader=FileSystemLoader(os.getcwd(), encoding='cp1252'))

        ## Get context for content types

        projectfilepath = os.path.realpath(recipe if genproject.sourcepath is None else os.path.join(genproject.sourcepath, recipe))
        genprojectxmls.append(projectfilepath) # add to ignore list for copy
        os.chdir(os.path.dirname(os.path.realpath(projectfilepath)))
        root = ET.parse(projectfilepath).getroot()

        # Project name
        context['projectname'] = os.path.basename(projectfilepath).split('.')[0] # backup solution is filename of xml description
        if root.find('./name') is not None: context['projectname'] = root.find('./name').text # Prefer tag <name>

        # Debugger connection
        context['debuggerConnection'] = root.findall('.//debuggerConnection')[0].text

        # Project attributes (post/pre build or anything)
        context['projectAttributes'] = []
        for attr in root.findall('.//attribute'):
            context['projectAttributes'].append(attr.text)

        # Is it a TI-RTOS / RTSC project?
        if root.find('.//isTIRTOS') is not None:
            context['isTIRTOS'] = True if root.find('.//isTIRTOS').text.lower() == 'true' else False

        ## Linker options
        context['linkeroptions'] = []
        for attr in root.findall('.//linkeroption'):
            context['linkeroptions'].append(attr.text)
        for f in sorted([k for k in root if k.tag == 'linkeroptions'], key=lambda x: int(x.attrib.get('level', 0)), reverse=True):
            # f is include file sorted by presedence, highest first
            includedXml = f.attrib.get('include', None)
            if includedXml is not None and os.path.exists(includedXml):
                genprojectxmls.append(os.path.realpath(includedXml))
                included = ET.parse(includedXml).getroot() if includedXml is not None else []
                for d in [xd for xd in included if xd.tag == 'linkeroption']:
                    context['linkeroptions'].append(d.text)


        # Project references
        context['projectrefs'] = []
        refs = root.findall('./projectrefs/ref')
        if len(refs) > 0:
            for ref in refs:
                context['projectrefs'].append(ref.text)
                if ref.attrib.get('boundary', None) is not None:
                    context['referenced_boundary_project'] = ref.text

        ## Linked libraries
        ## Find all include references, sort by reverse level of precedence, add to global context
        context['libraries'] = []
        for attr in root.findall('.//lib'):
            context['libraries'].append(Library(attr.text))
        for f in sorted([k for k in root if k.tag == 'libs'], key=lambda x: int(x.attrib.get('level', 0)), reverse=True):
            # f is include file sorted by presedence, highest first
            includedXml = f.attrib.get('include', None)
            if includedXml is not None:
                genprojectxmls.append(os.path.realpath(includedXml))
            included = ET.parse(includedXml).getroot() if includedXml is not None else []
            for d in [xd for xd in included if xd.tag == 'lib']:
                path = cgi.escape(d.text, True)
                context['libraries'].append(Library(path))

        ## Defines
        ## Find all include references for defines, sort by level of precedence, add to global context, ensuring unique entires, split by = if present
        context['defines'] = OrderedDict()
        for f in sorted([k for k in root if k.tag == 'defines'], key=lambda x: int(x.attrib.get('level', 0))):
            # f is include file sorted by presedence, lowest first
            includedXml = f.attrib.get('include', None)
            if includedXml is not None:
                genprojectxmls.append(os.path.realpath(includedXml))
            included = ET.parse(includedXml).getroot() if includedXml is not None else []
            for d in [xd for xd in included if xd.tag == 'define']:
                expression = d.text
                context['defines'][expression.split('=')[0]] = Define(expression) # Add text
        # Todo, add from project file
        for expression in root.findall('.//define'):
            context['defines'][expression.text.split('=')[0]] = Define(expression.text)
        context['defines'] = list(context['defines'].values()) # Flatten to list


        ## Includepaths
        ## Find all include references, sort by reverse level of precedence, add to global context
        context['includes'] = []
        for attr in root.findall('.//include'):
            context['includes'].append(Include(attr.text))
        for f in sorted([k for k in root if k.tag == 'includes'], key=lambda x: int(x.attrib.get('level', 0)), reverse=True):
            # f is include file sorted by presedence, highest first
            includedXml = f.attrib.get('include', None)
            if includedXml is not None:
                genprojectxmls.append(os.path.realpath(includedXml))
            included = ET.parse(includedXml).getroot() if includedXml is not None else []
            for d in [xd for xd in included if xd.tag == 'include']:
                path = d.text
                context['includes'].append(Include(path))
        # Now make sure everything relative to {PROJECT_LOC} wins the debate.
        topincludes = []
        includes = []
        for i in context['includes']:
            if 'PROJECT_LOC' in i.path:
                topincludes.append(i)
            else:
                includes.append(i)
        topincludes.extend(includes)
        context['includes'] = topincludes

        ## Pathvars
        ## Find all include references, sort by reverse level of precedence, add to global context, ensuring unique entities
        context['pathvars'] = OrderedDict()
        for f in sorted([k for k in root if k.tag == 'pathvars'], key=lambda x: int(x.attrib.get('level', 0))):
            # f is include file sorted by presedence, highest first
            includedXml = f.attrib.get('include', None)
            if includedXml is not None:
                genprojectxmls.append(os.path.realpath(includedXml))
            included = ET.parse(includedXml).getroot() if includedXml is not None else []
            for d in [xd for xd in included if xd.tag == 'pathvar']:
                name = d.find('name').text if d.find('name') is not None else 'NOT_DEFINED'
                path = d.find('path').text if d.find('path') is not None else '.'
                context['pathvars'][name] = PathVar(name, path, 'project')

        # Use hard-coded values for cloud.
        if cloud:
            pathVars_cloud = [
            ("CC26XXWARE", "../../../../../../../../tirtos_simplelink_2_13_00_06/products/cc26xxware_2_21_01_15600", "project"),
            ("TI_RTOS_DRIVERS_BASE", "../../../../../../../../tirtos_simplelink_2_13_00_06/packages", "project"),
            ("TI_BLE_SDK_BASE", "../../../../../../../../simplelink/ble_cc26xx_2_01_00_44423_cloud", "project"),
            ("PROJECT_IMPORT_LOC", ".", "project"),
            ]
            context['pathvars'] = dict()
            for pv_name, pv_path, pv_scope in pathVars_cloud:
                context['pathvars'][pv_name] = PathVar(pv_name, pv_path, pv_scope)

        context['pathvars'] = list(context['pathvars'].values()) # Flatten to list

        ## Files
        ## Find all include references for files, sort by level of precedence, add to global context, ensuring unique entities
        context['files'] = OrderedDict()
        for f in sorted([k for k in root if k.tag == 'files'], key=lambda x: int(x.attrib.get('level', 0))):
            # f is include file sorted by presedence, lowest first
            includedXml = f.attrib.get('include', None)
            if includedXml is not None:
                genprojectxmls.append(os.path.realpath(includedXml))
            included = ET.parse(includedXml).getroot() if includedXml is not None else []
            for d in root.findall('.//file') + [xd for xd in included if xd.tag == 'file']:
                filedict = dict([(elem.tag, elem.text) for elem in d])
                for k, v in filedict.items():
                    filedict[k] = str(Template(v).render(context)) # Run template.
                unique = filedict['target']+filedict['source'].split('/')[-1]
                context['files'][unique] = File(**filedict) # Overwrites lower pri if exists, and uses defaults for File class if not in xml.
        context['files'] = list(context['files'].values()) # Flatten to list

        ## CompilerOptions
        ## Find all include references, sort by level of precedence, add to global context
        context['compileroptions'] = []
        for attr in root.findall('.//compileroption'):
            context['compileroptions'].append(str(Template(attr.text).render(context)))
        for f in sorted([k for k in root if k.tag == 'compileroptions'], key=lambda x: int(x.attrib.get('level', 0))):
            # f is include file sorted by presedence, lowest first
            includedXml = f.attrib.get('include', None)
            if includedXml is not None:
                genprojectxmls.append(os.path.realpath(includedXml))
            included = ET.parse(includedXml).getroot() if includedXml is not None else []
            for d in [xd for xd in included if xd.tag == 'compileroption']:
                option = d.text
                option = str(Template(option).render(context)) # Run template
                context['compileroptions'].append(option)

        templates = [env.get_template(t) for t in env.list_templates(filter_func=lambda x: any([t in x for t in genproject.templates]))]

        for t in templates:
            outString = t.render(context).encode('cp1252')
            outFile = projectfilepath.split('.xml')[0]+'.projectspec'
            f = open(outFile, 'w')
            print "Writing", os.path.basename(outFile)
            f.write(outString)
            f.flush()
            f.close()
    # End for recipe.. Inserting #end comments is perhaps suggestive that a refactoring is in order

    # Copy generated files to outpath
    if genproject.outpath is not None:
        outpath = os.path.join(genproject.outpath, genproject.name)
        outpath = os.path.realpath(outpath)
        sourcepath = os.path.join(genproject.sourcepath, genproject.name)

        print "Copying files from >>", os.path.relpath(sourcepath, os.path.commonprefix([outpath,sourcepath])), "to >>", os.path.relpath(outpath, os.path.commonprefix([outpath,sourcepath]))

        def copyfile(out, dirname, names):
            reldir = os.path.relpath(dirname, sourcepath)
            for n in [x for x in names if not os.path.join(dirname, x) in genprojectxmls and not os.path.isdir(os.path.join(dirname, x))]:
                mkdir_p(os.path.normpath(os.path.join(out, reldir)))
                shutil.copyfile(os.path.join(sourcepath, reldir, n), os.path.normpath(os.path.join(out, reldir, n)))
                #print "%s --> \n%s\n" %(os.path.join(sourcepath, reldir, n), os.path.normpath(os.path.join(out, reldir, n)))

        os.path.walk(sourcepath, copyfile, outpath)
# End for genproject
