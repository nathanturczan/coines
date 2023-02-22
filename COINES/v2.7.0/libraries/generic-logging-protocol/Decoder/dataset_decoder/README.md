To run this script perform below steps.
 1. Ensure 'Anaconda' is installed in your PC. You can install it from IT services
 2. Double click on `./setup_conda_env.bat` file. (Tested only on Windows)
 3. It will prompt you to enter a name of your choice for the conda environment that is to be created.
 4. This will create a new conda environment and install all necessary packages for running

# For more details refer the datahandling script video.
Note: Need to import entire decoder folder in Jupyter notebook for the execution of dataset decoder.
1. Copy and paste the Dataset from phone to dataset_decoder folder. 
2. Make sure necessary binaries from deviceare placed into bin folder of respective dataset.
3. The entire Decoder folder from release package along with dataset has to be imported in to jupyter notebook. 
   The easy way to do is to find from where Jypter notebook is executing and paste the decoder folder.
   It can be found by running following python code in juypter notebook
   import os
   print(os.getcwd())
4. Make sure juypter notebook kernel is using the conda environment which we had setup.
5. Open the datalogger_tutorial.ipynb file available in Decoder/datset_decoder folder of release package in juypter notebook.
6. Replace the tutorial name with dataset name to be tested in the line "datasetList.append('tutorial\\')"
7. Save and execute each cell and at last all the bin files in dataset will be converted to csv file. 
   The csv file will be avaialble in csv folder and plots will be avaialble in plots folder.