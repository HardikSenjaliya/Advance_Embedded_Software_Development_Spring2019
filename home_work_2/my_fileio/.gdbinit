target extended-remote 128.138.189.206:2345
remote put fileio_pkg fileio_pkg
set remote exec-file fileio_pkg
set sysroot /home/hardyk/buildroot/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/
file fileio_pkg
b 39
run

