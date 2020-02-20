#!/usr/bin/env sh
if [ "$#" -ne 1 ]; then
    echo "No KML file specified. Exiting!"
    exit 1
fi

# Prepare and run
export RUN_LIVE_VIEW_YAML_FILE_PATH=$1
jupyter nbconvert --to notebook --execute --ExecutePreprocessor.timeout=None map_vis_live.ipynb
