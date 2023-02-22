::Temporarily adds Anaconda3 path in the environmental path
SET PATH=%PATH%;"C:/Program Files/Anaconda3/Scripts"
set /p ENVNAME=Enter the name of the conda environment you would like to create:
::Creates a conda environment with the given name
call conda create --name %ENVNAME% python -y
::Activates the created environment
call conda activate %ENVNAME%
::Installs ipykernel
call conda install ipykernel python -y
call ipython kernel install --name %ENVNAME%
::Installs all the necessary packages using pip
call pip install matplotlib
call pip install pandas
call pip install numpy
call pip install datetime