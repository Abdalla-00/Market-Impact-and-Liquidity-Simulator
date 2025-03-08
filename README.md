# Blank Cadmium Project (RT)
|  Linux | Windows| ESP32 | MSP432 |
|:--|:--|:--|:--|
|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:question:|
## Introduction
This is a blank project that you can use as a template to start off your DEVS models. This project consits of a template atomic and coupled model.

## Dependencies
This project assumes that you have Cadmium installed in a location accessible by the environment variable $CADMIUM.
_This dependency would be met by default if you are using the ARSLAB servers. To check, try `echo $CADMIUM` in the terminal_

## Build
To build this project, run:
```sh
source build_sim.sh
```
__NOTE__: Everytime you run build_sim.sh, the contents of `build/` and `bin/` will be replaced.

To build this project for the ESP32, run:
```sh
source build_esp.sh
```

## Execute
To run the project, run:
```sh
./bin/sample_project
```

To flash the project onto the esp32, run:
```sh
idf.py -p $ESP_PORT flash
```

## Modify
You can modify this project per your requirement. Change the project name defined in the topmost CMakeLists.txt file here:
```cmake
set(projectName "sample_project")
```
If you want to add other include directories, add the following to the CMakeLists.txt file in the `main` directory:
```cmake
target_include_directories(${projectName} PRIVATE "/path/to/dependency")
```
