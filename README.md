A "Simple" example of a Intel VT Hypervisor

**This hypervisor is designed to load at runtime on Windows 10, when the driver loads, it activates virtualization and "subverts" the running Windows OS. <br><br>
The kernel driver Becomes a lightweight hypervisor and the already running Windows 10 actually becomes a VM. You can use this along with EPT to hook system functions WITHOUT triggering Patchguard. <br>
It can also be used to debug specific processes. The driver is not signed. You need to disable driver signiture enforcement to load it!**

This is a simple version of what I implemented for my **Master's thesis project**. It doesn't do much but some VM_EXIT cases are handled. <br>
The above applications are not implemented in this simple example, but very possible. This is meant to be a tool to learn from.
