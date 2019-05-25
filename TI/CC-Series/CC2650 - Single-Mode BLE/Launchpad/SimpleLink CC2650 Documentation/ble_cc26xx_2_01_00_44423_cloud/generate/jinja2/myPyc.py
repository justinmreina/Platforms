from System import IO as IO
import os

def walk(folder):
  for file in IO.Directory.GetFiles(folder):
    yield file
  for folder in IO.Directory.GetDirectories(folder):
    for file in walk(folder): yield file

folder = os.path.dirname(os.path.abspath(__file__))

#pygments_files = list(walk(IO.Combine(folder, 'pygments')))
#pygments_dependencies = list(walk(Combine(folder,'pygments_dependencies')))

all_files = [f for f in walk(folder) if f[-2:] == 'py']
#print all_files
#pygments_files + pygments_dependencies
#all_files.append(IO.Path.Combine(folder, 'devhawk_formatter.py'))

import clr
clr.CompileModules(folder +".dll", *all_files)
