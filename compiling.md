## gcc row

```bash
gcc main.c $(pkg-config --cflags --libs python3) -o helloworld
```
```
cc -DPD -I "../pure-data/src/" -I/usr/include/python3.7m -I/usr/include/x86_64-linux-gnu/python3.7m -lpython3.7m -DUNIX  -fPIC  -Wall -Wextra -Wshadow -Winline -Wstrict-aliasing -O3 -ffast-math -funroll-loops -fomit-frame-pointer -march=core2 -mfpmath=sse -msse -msse2 -msse3 -o hworld.o -c hworld.c
```

gcc newpy.c $(pkg-config --cflags --libs python3) -o newpy

cc -DPD -I "/home/neum/Documenti/pure-data/src/" -I/usr/include/python3.7m -I/usr/include/x86_64-linux-gnu/python3.7m -lpython3.7m -DUNIX  -fPIC  -Wall -Wextra -Wshadow -Winline -Wstrict-aliasing -O3 -ffast-math -funroll-loops -fomit-frame-pointer -march=core2 -mfpmath=sse -msse -msse2 -msse3 -o newpy.o -c newpy.c