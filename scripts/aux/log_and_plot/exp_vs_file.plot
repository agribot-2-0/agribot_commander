set terminal svg
set datafile separator ","
plot "exp_data.csv" u 2:1, "file_data.csv" u 2:1
