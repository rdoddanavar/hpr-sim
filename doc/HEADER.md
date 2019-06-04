# hpr-sim

`High Power Rocketry - Flight Simulation`

## File Header Format

Block comment(s)
 - Python (`*.py`)
   - Docstring style
   - Block start: `"""`
   - Block end: `"""`
 - C++ (`*.cpp`, `*.h`)
   - Block start: `/*`
   - Block end: `*/`
 - Bash (`*.sh`), Makefile, & YAML (`*.yaml`)
   - All lines prefixed with `#`

"Description"
 - Give brief outline of file's purpose w/ regards to overall program
 - Example:
   - df

"Functions"
 - List any declared functions, include argument & return types
 - Omitted for Bash/Makefile/YAML files 
 - Example:
```
Functions:
    foo1()           -> float
    foo2(int, float) -> str
    foo3(str)        -> 
```
  
"Classes"
 - List any declared classes, include base class if applicable
 - Omitted for Bash/Makefile/YAML files 
 - Example:
```
Classes:
    Foo
    FooBar(Base)
```

"Dependencies"
 - List any external dependencies, i.e. any files referenced or imported
 - Do not list files dependent on current file
 - Example:
```
Dependencies:
    hpr-sim/subdir/dep1.py
                   dep2.yaml
```

### Template: Python

```
"""
High Power Rocketry - Flight Simulation
GNU General Public License v3.0
Copyright (C) 2019 Roshan Doddanavar
https://rdoddanavar.github.io

██╗  ██╗██████╗ ██████╗       ███████╗██╗███╗   ███╗
██║  ██║██╔══██╗██╔══██╗      ██╔════╝██║████╗ ████║
███████║██████╔╝██████╔╝█████╗███████╗██║██╔████╔██║
██╔══██║██╔═══╝ ██╔══██╗╚════╝╚════██║██║██║╚██╔╝██║
██║  ██║██║     ██║  ██║      ███████║██║██║ ╚═╝ ██║
╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝      ╚══════╝╚═╝╚═╝     ╚═╝

Path:
    hpr-sim/<dir>/.../<filename>.<ext>
Created:
    2019-MM-DD
Type:
    Python3 <module/script>
Description:
    < >
Functions:
    < >
Classes:
    < >
Dependencies:
    hpr-sim/<dir1>/<dep1>.<ext>
                   <dep2>.<ext>
"""
```

### Template: C++ 

```
/*
High Power Rocketry - Flight Simulation
GNU General Public License v3.0
Copyright (C) 2019 Roshan Doddanavar
https://rdoddanavar.github.io

██╗  ██╗██████╗ ██████╗       ███████╗██╗███╗   ███╗
██║  ██║██╔══██╗██╔══██╗      ██╔════╝██║████╗ ████║
███████║██████╔╝██████╔╝█████╗███████╗██║██╔████╔██║
██╔══██║██╔═══╝ ██╔══██╗╚════╝╚════██║██║██║╚██╔╝██║
██║  ██║██║     ██║  ██║      ███████║██║██║ ╚═╝ ██║
╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝      ╚══════╝╚═╝╚═╝     ╚═╝

Path:
    hpr-sim/<dir>/.../<filename>.<ext>
Created:
    2019-MM-DD
Type:
    C++11 <header/source>
Description:
    < >
Functions:
    < >
Classes:
    < >
Dependencies:
    hpr-sim/<dir1>/<dep1>.<ext>
                   <dep2>.<ext>
*/
```

### Template: Bash/Makefile/YAML

```
# High Power Rocketry - Flight Simulation
# GNU General Public License v3.0
# Copyright (C) 2019 Roshan Doddanavar
# https://rdoddanavar.github.io
# 
# ██╗  ██╗██████╗ ██████╗       ███████╗██╗███╗   ███╗
# ██║  ██║██╔══██╗██╔══██╗      ██╔════╝██║████╗ ████║
# ███████║██████╔╝██████╔╝█████╗███████╗██║██╔████╔██║
# ██╔══██║██╔═══╝ ██╔══██╗╚════╝╚════██║██║██║╚██╔╝██║
# ██║  ██║██║     ██║  ██║      ███████║██║██║ ╚═╝ ██║
# ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝      ╚══════╝╚═╝╚═╝     ╚═╝
# 
# Path:
#     hpr-sim/<dir>/.../<filename>.<ext>
# Created:
#     2019-MM-DD
# Type:
#     <Bash/Makefile/YAML> <script/config file>
# Description:
#     < >
# Dependencies:
#     hpr-sim/<dir1>/<dep1>.<ext>
#                    <dep2>.<ext>
```

