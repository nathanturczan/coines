# GLP Decoder

## There are different ways to decode the .bin file to .csv
#### 1.Using `GLP_Decoder.exe`
If you don't have python installed you can use `./GLP_Decoder.exe` to convert the .bin files to .exe. The GLP_Decoder.exe is available in `/exe` folder.
Refer `USAGE` section for more details
#### 2.Using `GLP_Decoder.py`
 If you are a developer and need to modify the GLP then the source file is  `./GLP_Decoder.py`. To run this script perform below steps.
 1. Ensure 'Anaconda' is installed in your PC. You can install it from IT services
 2. Double click on `./setup_conda_env.bat` file. (Tested only on Windows)
 3. It will prompt you to enter a name of your choice for the conda environment that is to be created.
 4. This will create a new conda environment and install all necessary packages for running GLP_Decoder.py script.
 5. Now open command prompt and navigate to the location where the source file is located.
 6. Activate the conda environment by using command `activate <environment name>
 7. Execute the command eg : `python GLP_Decoder.py --help` . Modify the command based on usage.
	Refer `USAGE` section for more details.
	
If the conda environment is already created then just do the steps from 5.) to 7.). 
You can list the all conda environment using the command `conda env list` (Incase you have forgotten the name of the conda environment you have created before :smiley: )

#### Usage

`<python GLP_Decoder.py>/<GLP_Decoder.exe> [-h] [-f FILENAME] [-p] [-c] [-s] [-o OUTPUT_FILE]`

| Arguments               | Description                     | 
|-------------------------|:-------------------------------:|
| -h, --help              | show this help message and exit |
| -f,--file-name          | Specify BIN file name           |
| -p,--plot-data    	  | Plot data if specified          |
| -c,--show-packet-count  | Show packet count if specified  |
| -s,--save-plot          | Save plot to PNG file           |
| -o,--output-file        | Save data to CSV/JSON file      |

##### Examples
1.Convert BIN file to CSV  : `<python GLP_Decoder.py>/<GLP_Decoder.exe> -f bma4xy_1564050444.bin -o bma4xy_1564050444.csv`

2.Convert BIN file to JSON : `<python GLP_Decoder.py>/<GLP_Decoder.exe> -f bma4xy_1564050444.bin -o bma4xy_1564050444.json`

3.Plot data from BIN file  : `<python GLP_Decoder.py>/<GLP_Decoder.exe> -f bma4xy_1564050444.bin -p`

4.Save plot                : `<python GLP_Decoder.py>/<GLP_Decoder.exe> -f bma4xy_1564050444.bin -s`


# How to generate GLP_Decoder.exe
If the GLP_Decoder.py is modified,then for generating GLP_Decoder.exe ,follow the below steps:
1. Setup conda environment.
2. Open anaconda prompt and navigate to where GLP_Decoder.py is placed
3. Create conda environment `conda create --name <ENVNAME>` 
4. Activate conda environment `conda activate <ENVNAME>`
5. Install only the packages that are necessary to run the GLP_Decoder.py script `pip install matplotlib` , `pip install numpy`, `pip install datetime`
6. Either follow steps 1 to 5 or execute ./setup_conda_env.bat. If conda environment is created, then open Anaconda prompt and activate it by calling `conda activate <ENVNAME>`
6. Install `pip install pyinstaller`
7. Now generate `GLP_Decoder.exe` by giving `pyinstaller --onefile GLP_Decoder.py`
8. The exe file gets generated inside a folder called `dist`

Ensure only necessary packages are installed in the conda environment else the .exe file that gets generated will be huge
