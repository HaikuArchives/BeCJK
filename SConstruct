################################
# main building script for BeCJK
################################


#################################
#   global configs for building
#################################
# flag_package = 'package' or 'non-package'
flag_package = 'non-package'

#################################
#   path for library and BeCJK & dicts
#################################
besavager_path = ''
becjk_addon = ''
dicts_path = ''

env = Environment()

Export("env")

if('package' == flag_package):
    besavager_path = '/boot/system/lib/besavager'
    becjk_addon = '/boot/system/add-ons/input_server/methods/BeCJK'
    dicts_path = '/boot/system/data/BeCJK/dicts'
elif('non-package' == flag_package):
    besavager_path = '/boot/system/non-packaged/lib/besavager'
    becjk_addon = '/boot/system/non-packaged/add-ons/input_server/methods/BeCJK'
    dicts_path = '/boot/system/non-packaged/data/BeCJK/dicts'
else:
    besavager_path = './tmp/besavager'
    becjk_addon = './tmp/BeCJK'
    dicts_path = './tmp/dicts'
    

# besavager install
besavager_script = '#besavager/SConscript'
besavager_files= SConscript(besavager_script, variant_dir = 'build/besavager')
env.Install(besavager_path, besavager_files);

# becjk install
becjk_script = '#src/SConscript'
becjk_file = SConscript(becjk_script, variant_dir='build/BeCJK')
env.InstallAs(becjk_addon, becjk_file)

# dicts install
dicts_script = '#data/dicts/SConscript'
dicts_files = SConscript(dicts_script, variant_dir='build/dicts')
env.Install(dicts_path, dicts_files)

