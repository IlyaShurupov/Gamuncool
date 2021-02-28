
Import('cbld')

Includes = [cbld.path['ROOT'] + '/Sourse/Types', cbld.path['ROOT'] + '/Sourse/Application'] 
Files = ['private/main.cpp']
Libs = [
	'Types', 
	'Application', 
	'd2d1',  
	'kernel32', 
	'user32',
	'gdi32',
	'winspool',
	'comdlg32',
	'advapi32',
	'shell32',
	'ole32',
	'oleaut32',
	'uuid',
	'odbc32',
	'odbccp32',
	]

LibsPath = [cbld.path['OUT_ABS'] + '/Sourse/Types', cbld.path['OUT_ABS'] + '/Sourse/Application', cbld.path['ROOT'] + '/Externals']

exe = cbld.Executable('DrawApp', Files, Includes, Libs, LibsPath)

out = cbld.path['OUT_ABS'] + '/Examples/private' 
input = cbld.path['ROOT'] + '/Configuration'
Command(out, input, Copy("$TARGET", "$SOURCE"))

