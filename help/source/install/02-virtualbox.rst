Create a VirtualBox VM
==================

Hollywood is tested and known to work in Oracle VirtualBox.  VirtualBox is a free tier 2 hypervisor available under the GPL and the VirtualBox PUEL.

These are the general steps and configuration variables required for a virtual machine running Hollywood.

The Quick Jist
----------------
If you're seasoned with virtualization here's what you need to know:
> * Chipset: ICH9
> * Cores/RAM: 2+ / 2GB+
> * Enable EFI
> * Enable 3D acceleration, max out VRAM.
> * Use Intel HD Audio instead of AC97 or SB16.

We recommend using 64GB disks to allocate enough space for the system, applications and data.  The Hollywood base system currently utilizes about 2GB of active space.  You may create your disks accordingly with a bare minimum of 4GB.

Create your VirtualBox VM
----------------

Select the *Machine -> New...* Option in the Oracle VM VirtualBox Manager.  You will be asked to name the VM and select the folder.  For the **Type** select Linux.  Most options of **Version** will work fine we advise selecting Ubuntu (64-bit) to pre-select most relevant settings.  Click on Next.

You will be asked for a memory allocation with a suggested value likely to be below Hollywood minimum specifications.  Specify **at least** 2048 MB of memory or more, click next.

You will be asked to create a virtual hard disk, again with a suggested value below specifications.  Select *Create a virtual hard disk now* and click Create.  For your hard disk file type, continue with **VDI** *(VirtualBox Disk Image)*.  We recommend using a *Dynamically Allocated* disk.  On the final step of virtual disk creation, select a size of at least 64 GB or higher.  **NOTE:** With a dynamically allocated disk, the entire size will *not* be initially utilized on your hard disk, and the space will be available for other needs on your host OS.

The VirtualBox VM for Hollywood will be available in the list now, however you will need to adjust additional settings.  Select the VM and select the Settings toolbar button.

Under **System** - **Motherboard**:
> * Change *Chipset* to: ICH9
> * Change *Pointing Device* to: USB Tablet
> * Check *Enable EFI (special OSes only)*

Under **System** - **Processor**:
> * Change *Processor(s)* to a minimum of 2.

Under **Display** - **Screen**:
> * Change *Video Memory:* to 128MB.
> * Change *Graphics Controller* to: VMSVGA
> * Check *Enable 3D Acceleration*.

Under **Audio** - **Screen**:
> * Change *Audio Controller* to: Intel HD Audio.
 
 Your settings are now configured properly for Hollywood and you may start the VM from the ISO image.
