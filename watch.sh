#!/usr/bin/env fish

set plugin_name deepface_filter
set plugin_so build/$plugin_name.so
set obs_plugin_dir /home/ln64/.local/share/obs-studio/plugins

echo "Watching for changes..."

watchexec -e cpp,h --restart --clear -- fish -c "
  echo '[BUILDING]'

  if not test -d build
    mkdir build
  end

  cd build
  and cmake ..
  and make -j(nproc)

  cd ..

  if test -f build/$plugin_name.so
    echo '[COPYING] build/$plugin_name.so → $obs_plugin_dir'
    cp build/$plugin_name.so $obs_plugin_dir
  else
    echo '[ERROR] Build failed, .so not found.'
  end
"
ln64 ALL=(ALL) NOPASSWD: /bin/cp /home/ln64/Source/deepface/deepface-filter/build/deepface_filter.so /usr/lib/obs-plugins/

# OBS_PLUGIN_PATH="$HOME/.local/share/obs-studio/plugins" obs