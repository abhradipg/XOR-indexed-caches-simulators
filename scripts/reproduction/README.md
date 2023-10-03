## Running warping cache simulation with PolyBench examples

Prerequisite:
```
./prepare-polybench.sh
```
### Run Warping Cache Simulation for all the PolyBench examples by
```
./run-polybench-all.sh <dataset> <warping> <heuristics> <cache-config>
```


| Argument               | Description                  | Possible Values                                                            |
|------------------------|------------------------------|----------------------------------------------------------------------------|
| \<dataset\>            | PolyBench dataset option     | small, medium, large, xlarge, tiled                                    |
| \<warping\>            | Apply warping                | true, false                                                                |
| \<heuristics\>         | Use heuristics while warping | true, false                                                                |
| \<cache_config\> | Cache config file   | any file from cache-configs dir |

Example:
```
./run-polybench-all.sh small false false cache-configs/cascadelake-l1.config
```

#### Disable specific PolyBench kernels
You can disable specific PolyBench kernels by editing the polybench-kernels.config file.

Any kernel source added to disabled_polybench_src_list there won't be run by run-polybench-all.sh.

### Run Warping Cache Simulation for a single PolyBench example
```
./run-polybench-single.sh <example> <dataset> <warping> <heuristics> <cache-config>
```
where possible values for \<example\> are PolyBench kernel names: heat-3d, fdtd-2d, 2mm...

Example:
```
./run-polybench-single.sh heat-3d large true true cache-configs/cascadelake-l1.config
```
