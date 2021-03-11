
from common import * 

def GenObj(file, incl, output, dbg = False, sysarch = "64", platform = 'windows', defenitions = []):
	if platform == 'windows' or platform == 'linux':
		outfile = output + "/" + file.rsplit('/', 1)[1] + '.o'	
		if not os.path.isdir(output):
			os.makedirs(os.path.abspath(output))
		cmd = "g++ -c " 
		if dbg:
			cmd = cmd + " -g "
		sysarch_arg = " -m" + sysarch + " "
		destr = to_str(defenitions, True, " -D")
		cmd = cmd + sysarch_arg + file + '.cpp '+ to_str(incl, True, ' -I') + ' -o ' + outfile + destr
		os.system(cmd)
		print("      " +  file.rsplit('/', 1)[1] + '.o')

def PackObjs(files, outdir, name, platform = 'windows'):
	if platform == 'windows' or platform == 'linux':
		cmd = "ar rcs " 
		cmd = cmd + outdir + '/' + name + '.lib ' + to_str(files, False, ' ', True, '.o ')
		os.system(cmd)

def LinkObjs(name, output, objs, objpaths, dbg = False, sysarch = "64", platform = 'windows'):
	if platform == 'windows' or platform == 'linux':
		outfile = " -o " + output + "/" + name + "/" + name
		libstr = to_str(objs, True, " -l")
		libpathsstr = to_str(objpaths, True, " -L")
		cmd = "g++ -static " 
		if dbg:
			cmd = cmd + " -g "
		sysarch_arg = " -m" + sysarch + " "
		cmd = cmd + sysarch_arg + libpathsstr + libstr + outfile
		os.system(cmd)