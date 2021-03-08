import os as os
import cparser


class ExeptionTerminated(Exception):
	pass

class ExeptionBuildError(Exception):
	pass


class BuildCommand():
	
	def init(this, name, describ):
		this.id = name
		this.describ = describ

	def OnError(this, args):
		print(" Failed to execute command ")

	def OnFinish(this):
		pass

	def Exec(this, bld, args, original):
		pass

	def help(this):
		print("  " + this.id + " - " + this.describ)

class BLD(BuildCommand):
	def __init__(this):
		this.init("bld", " compiles binaries depending on the env ")
	
	def Exec(this, bld, args, original):
		bld.Build(args)

class CLEAR(BuildCommand):
	def __init__(this):
		this.init("clr", " removes output dir recursevly")
	
	def Exec(this, bld, args, original):
		from main import RootDir
		import shutil
		outdir = RootDir(bld.env.RootDirName) + "\\" + bld.env.OutDir
		if os.path.isdir(outdir):
			shutil.rmtree(os.path.abspath(outdir))

class REBLD(BuildCommand):
	def __init__(this):
		this.init("rbld", " recompiles binaries depending on the env ")
	
	def Exec(this, bld, args, original):
		cache = cparser.load_from_json(os.path.dirname(__file__) + "\\cache.json")
		cache['LastRun'] = 0
		cparser.save_to_json(cache, "cache", os.path.dirname(__file__))
		bld.Build(args)

class ENV(BuildCommand):
	def __init__(this):
		this.init("env", "prints the env variables")
	
	def Exec(this, bld, args, original):
		if len(args) == 1:
			if "list" in args[0]:
				for env in bld.envs:
					env.Log()	
			else:
				found = False
				for env in bld.envs:
					if env.name == args[0]:
						bld.env = env
						found = True
				if not found:
					print("  not found")

		elif not len(args):
			bld.Logout("   current build env: ")
			bld.env.Log()

class SAVE(BuildCommand):
	def __init__(this):
		this.init("save", " saves the current env ")
	
	def Exec(this, bld, args, original):
		cparser.save_to_json(bld.env.__dict__,  args[1], os.path.dirname(__file__) + "\\" + args[0])

class LOAD(BuildCommand):
	def __init__(this):
		this.init("load", " loads spicified env and sets it as current ")
	
	def Exec(this, bld, args, original):
		from main import Env 
		newenv = Env()
		newenv.__dict__ = cparser.load_from_json(os.path.dirname(__file__) + "\\" + args[0] + "\\" + args[1] + ".json")
		bld.env = newenv
		bld.envs.append(newenv)

class EXIT(BuildCommand):
	def __init__(this):
		this.init("q", " exits from builder commands proc ")
	
	def Exec(this, bld, args, original):
		exit(0)

class HELP(BuildCommand):
	def __init__(this):
		this.init("help", " prints describtin of each command  ")
	
	def Exec(this, bld, args, original):
		for command in bld.commands:
			command.help()

class DBG(BuildCommand):
	def __init__(this):
		this.init("dbg", " runs compiler debuging utils ")
	
	def Exec(this, bld, args, original):
		if os.path.isfile(args[0]):
			os.system("gdb " + args[0])
		else:
			exe = args[0]
			files = []
			cparser.FindFiles(files, os.path.abspath(bld.path['OUTPUT']), 'exe', True, args[0])
			if not len(files):
				print(" exe not found in bld root path ")
			else:
				if len(files) > 1:
					bld.Logout(" Ambigues Executable Name - Enter full path", "warning")
				else:
					os.system("gdb " + files[0])

class SET(BuildCommand):
	def __init__(this):
		this.init("envset", " set enviroment variable ")
	
	def Exec(this, bld, args, original):
		exec("bld.env."+args[0]+"="+args[1])

class EXEC(BuildCommand):
	def __init__(this):
		this.init("exec", " executes python code directly calling 'exec'")
	
	def Exec(this, bld, args, original):
		exec(original.split(":", 1)[1].lstrip())

def init_comands(blder):
	blder.commands.append(ENV())
	blder.commands.append(BLD())
	blder.commands.append(EXIT())
	blder.commands.append(HELP())
	blder.commands.append(DBG())
	blder.commands.append(SET())
	blder.commands.append(EXEC())
	blder.commands.append(SAVE())
	blder.commands.append(LOAD())
	blder.commands.append(REBLD())
	blder.commands.append(CLEAR())