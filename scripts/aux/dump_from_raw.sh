#!/usr/bin/env sh
if [ "$#" -ne 1 ]; then
    echo "No file specified. Exiting!"
    exit 1
fi
python yaml_to_gps_csv.py $1 > file_data.csv
./extract_fields.sh > exp_data.csv
gnuplot exp_vs_file.plot > temporary_file_123456.svg
inkscape -z --export-background=ffffff -e out.png -w 1000 -h 1000 temporary_file_123456.svg
rm temporary_file_123456.svg
rm exp_data.csv
rm file_data.csv
