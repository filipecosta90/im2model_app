![alt text](https://raw.githubusercontent.com/filipecosta90/im2model/master/src/res/AppIcon128.png)


# Im2Model
Msc. Thesis regarding efficient computation to refine atomic models for TEM image simulation and matching 
********************************************************************************

### Metrics 
[![BCH compliance](https://bettercodehub.com/edge/badge/filipecosta90/im2model?branch=master)](https://bettercodehub.com/)

### Prerequisites

```
Boost 1.64.0
Opencv > 3.0
Bison
Flex
Qt5
```

## Installation
### Command Line Interface

```sh
$ git clone https://github.com/filipecosta90/im2model.git
$ cd im2model
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./Im2Model --help
```

### Graphical User Interface
Clone and open the project with Qt Creator, or:

```sh
$ git clone https://github.com/filipecosta90/im2model.git
$ cd im2model 
$ mkdir build
$ cd build
$ cmake .. -Dbuild_gui:BOOL=ON
$ make
$ ./Im2Model
```

## Authors

* **Filipe Costa Oliveira** 

## License

This project is licensed under the Apache2 License - see the [LICENSE.md](LICENSE.md) file for details


