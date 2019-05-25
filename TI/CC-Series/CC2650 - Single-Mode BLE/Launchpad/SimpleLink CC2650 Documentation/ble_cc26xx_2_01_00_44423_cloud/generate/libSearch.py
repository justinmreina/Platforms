#!/usr/bin/evn python

import argparse, os, sys, shutil, re, itertools
from lxml import etree

def inputArgExpansion(arg):
    if not os.path.exists(arg):
        arg = os.path.join(os.getcwd(), arg)
    if not os.path.exists(arg):
        sys.exit("<<< ERRROR >>>\nCannot find input file \"" + arg + "\"\n<<< /ERROR >>>")

    arg = os.path.abspath(arg)
    return arg

def parseXML(name,xmlTemplate):
    _params = []
    _nameExt = ""
    _relPath = ""
    _addconfig = ""


    with open(xmlTemplate,'r') as f:
        lxml_parser = etree.XMLParser(remove_blank_text=True)
        tree = etree.parse(f,lxml_parser)
        root = tree.getroot()
        libnodes = root.xpath("./library/name[text()=$libname]/..", libname = name )

        if len(libnodes) == 1:
            libnode = libnodes[0]

            if libnode.find('searchpath') is not None:
                _relPath = libnode.find('searchpath').text

            if libnode.find('outputnameadd') is not None:
                _nameExt = libnode.find('outputnameadd').text

            if libnode.find('matchconfig') is not None:
                _addconfig = libnode.find('matchconfig').text

            # Build Paramater for this Library
            nodes = libnode.xpath("./parameterlist/parameter")
            for node in nodes:
                define = node.find('define').text
                req = node.find('assign/required').text
                args = []
                argnodes = node.xpath("./assign/arg")
                for argnode in argnodes:
                    args.append(argnode.text)

                _params.append([define,req,args])

    return (_relPath,_nameExt,_addconfig,_params)

def libsToSearch(xmlTemplate):
    _libs = []

    with open(xmlTemplate,'r') as f:
        lxml_parser = etree.XMLParser(remove_blank_text=True)
        tree = etree.parse(f,lxml_parser)
        root = tree.getroot()

        nodes = root.xpath("./library")
        for node in nodes:
            _libs.append(node.find('name').text)

    return _libs

def parseOPT(opt,variables):
    _optparams = []
    comment = False

    with open(opt) as data:
        defines_found = []

        # Strip all commented items from opt file
        fline = data.readline()
        filestr = ''
        while fline:
            fline = fline.strip() + "\n"
            filestr += fline
            fline = data.readline()
        filestr = re.sub(r"(/\*(.|[\r\n])*?\*/)|(//.*)","",filestr)
        lines = filestr.split('\n')

        # Match any defines from xml that exist in opt
        for line in lines:
            for define, req, args in variables:
                if "-D"+define in line:
                    defines_found.append(define)
                    line = line.strip()
                    fragments = line.split("=")
                    if req == '0': #no assignment required
                        line = fragments[0]

                    else: #must check assignments and match those in cfg
                        if len(fragments) < 2:
                            print "<<< ERROR >>>"
                            print "Assignment required for " + fragments[0]
                            print "<<< /ERROR >>>"
                            sys.exit(1)
                        if fragments[1] not in args:

                            # Checking possible permutations : aka PING+PARAM_REQ or PARAM_REQ+PING
                            dargs = fragments[1].split("+")
                            permFound = False
                            for perm in itertools.permutations(dargs):
                                permstr = "+".join(perm)
                                if permstr in args:
                                    line = fragments[0] + "=" + permstr
                                    permFound = True

                            # No Permutations found so it must be incomplete
                            if not permFound:
                                print "<<< ERROR >>>"
                                print "Cannot match given assignment " + fragments[1] + " to any possible value:"
                                for arg in args:
                                    print "\t" + arg
                                print "<<< /ERROR >>>"
                                sys.exit(1)
                        line += '\n'
                    _optparams.append("\n"+line) #adding newline delimiters back for config match later - determines "//" existance

    #update list with variables that were left undefined
    for define, req, args in variables:
        if define not in defines_found:
            _optparams.append("\n//-D" + define)

    return _optparams

def matchLib(name,libdir,optparameters,config):
    libpath = ""

    for root, subFolders, filenames in os.walk(libdir):
        for filename in filenames:
            if ".opt" in filename:
                filestr = ""
                data = open(os.path.join(libdir,filename))
                line = data.readline()
                while line:
                    line = line.strip() + "\n"
                    filestr += line
                    line = data.readline()

                match = True
                for optparam in optparameters:
                    if optparam.startswith("\n//"):
                        defoptparam = optparam.replace("//","")
                        if optparam not in filestr and defoptparam in filestr:
                            match = False
                    else:
                        if optparam not in filestr:
                            match = False

                # I don't like this but we have nonuniform naming conventions...
                if match:
                    libname = filename.replace(".opt",".a")
                    if config != "":
                        libname = libname.replace(".a","_" + config + ".a")
                    print "<<< Using Library: " + libname + " >>>"
                    libpath = os.path.join(libdir,libname)

    return libpath

def main():
    parser = argparse.ArgumentParser(description='Used in conjunction with LPRF Stack Releases as an IAR prebuild step. Parses provided .opt file and adds corresponding library to project.')
    parser.add_argument("opt", help="Project local configuration file for Stack Project")
    parser.add_argument("xml", help="Generic template file that describes defines to match against in opt")
    parser.add_argument("libdir", help="Directory containing library files")
    parser.add_argument("libloc", help="Project local library file")
    parser.add_argument("config", nargs='?', default="FlashROM", help='Select whether the configuration requires FlashOnly or FlashROM library. Defaults to FlashROM')
    args = parser.parse_args()

    # Check input file existance and expand to absolute path
    args.opt = inputArgExpansion(args.opt)
    args.xml = inputArgExpansion(args.xml)
    args.libdir = inputArgExpansion(args.libdir)

    if not os.path.isabs(args.libloc):
        args.libloc = os.path.join(os.getcwd(),args.libloc)
    # Test if output directory exists. If not create it
    if not os.path.exists(os.path.dirname(args.libloc)):
        os.makedirs(os.path.dirname(args.libloc))

    #Parse XML strictly for libraries to search for, returns their names
    libs = libsToSearch(args.xml)

    for lib in libs:
        # Parse template file to determine config parameters to match against
        (relPath, nameExt, addConfig, params) = parseXML(lib,args.xml)
        path = os.path.join(args.libdir,relPath)
        config = ""
        if addConfig and addConfig != "0":
            config += args.config

        # Parse given opt file to determine state of parameters given in XML
        optparams = parseOPT(args.opt,params)

        # Search through all possible libraries to find lib that matches parameters in opt
        lib = matchLib(lib,path,optparams,config)

        copyTo = args.libloc
        if nameExt:
            idx = copyTo.rfind(".")
            copyTo = copyTo[:idx] + nameExt + copyTo[idx:]

        # Copy matched lib to the local project library
        if lib and os.path.exists(lib):
            shutil.copyfile(lib,copyTo)
        elif lib and not os.path.exists(lib):
            print "<<< ERRROR >>>"
            print "\tMatched the project .opt file to a library .opt file."
            print "\tHowever, unable to find corresponding library file \n\t" + lib
            print "<<< /ERROR >>>"
            sys.exit(1)
        else:
            print "<<< ERRROR >>>"
            print "Cannot match " + args.opt + " to any library .opt file in " + args.libdir
            print "\tSearched against the following defines in " + args.opt + ": "
            for optparam in optparams:
                print "\t\t\"" + optparam.strip() + "\""
            print "<<< /ERROR >>>"
            sys.exit(1)

if __name__ == '__main__':
    main()