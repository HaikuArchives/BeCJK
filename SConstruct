################################
# main building script for BeCJK
################################


script_besavager = '#besavager/SConscript'
script_becjk = '#becjk/SConscript'
script_data = '#data/SConscript'

SConscript(script_data)
SConscript(script_besavager, variant_dir = 'build/besavager')
SConscript(script_becjk, variant_dir='build/BeCJK')