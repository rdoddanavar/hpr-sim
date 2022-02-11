# hpr-sim

`High Power Rocketry - Flight Simulation`

## Input File Format

Input parameters are expressed using the YAML (`/ˈjæməl/`) data serialization format. It's human-readable, easy to modify, a provides a natural syntax for nested data structures. 

```
# Comment
---
group:
    param1: value
    param2:
        field1: value
        field2: value
    param3: [item1, item2, item3]
    param4: 
        - item1
        - item2
        - item3    
...
```

The preferred extension is `*.yaml`, but `*.yml` is also accepted.

### External Resources

 - General reference(s)
   - https://en.wikipedia.org/wiki/YAML
   - https://yaml.org/refcard.html
 - Online parser/validator
   - https://yaml-online-parser.appspot.com/
  
## Input Parameters

### Generic Input Parameter

```
param:
    value:
    min:
    max:
    unit:
    type:
    dist:
```

### Generic Input Name

```
name:
    value:
    path:
```

### Acceptable Units



### Available Distributions

### Group: Sim

|              | Param/Name | Min | Max   | Type |
|--------------|------------|-----|-------|------|
| `session`    | Name       |     |       |      |
| `modeRand`   | " "        |     |       |      |
| `modeProc`   | " "        |     |       |      |
| `numMC`      | Param      | 0   | inf   |      |
| `seedMaster` | " "        | 0   | 32767 |      |