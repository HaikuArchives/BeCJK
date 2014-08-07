# main building script for BeCJK

# path for library and BeCJK & dicts
besavager_path = './generated/lib/besavager'
becjk_addon = './generated/add-ons/input_server/methods/BeCJK'
dicts_path = './generated/data/BeCJK/dicts'

env = Environment()
Export("env")


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

