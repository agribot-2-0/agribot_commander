# GPS experiments scripts
Used to compare data from YAML file (generated from firebase) and data from ROS experiment.

#### Workflow:
1. Log data
2. Plot data **or** dump it to .png file

---

## Log data
`./log.sh`

Saves data from /agribot/gnss to *raw_log.csv* file

## Plot data  
`./plot_from_raw.sh /path/to/file.yaml`

Displays comparative plot of data from specified YAML file and *raw_log.csv* file created with `./log.sh` script


## Dump data to *out.png*  
`./dump_from_raw.sh /path/to/file.yaml`

Dumps comparative plot to *out.png* file. Running `./plot_from_raw.sh` is **not** required beforehand
