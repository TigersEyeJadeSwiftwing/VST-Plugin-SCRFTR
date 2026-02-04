Unpack your favorite version of the Irrlicht SDK in this folder, so that this readme shares the same folder with Irrlicht's readme, as well
as folders for bin, doc, include, lib, etc...

I hope to one day include a stripped version of the SDK (the whole SDK would take up too much file space), without the
example projects, sample media, and other stuff that isn't needed for ScorchCrafter plug-ins.

I haven't gotten permission for this yet, though, so until then you will need to download the Irrlicht SDK yourself.
(It is on SourceForge, you can also Google it, and it is FREE.)

*NOTE that the SC code has not been tested with an SDK version earlier than 1.7.2.

*ALSO NOTE That you may need to edit the compile config file in the SDK to prevent it from trying to include and use DirectX.
(SC plug-ins by default use a quick software renderer, although OpenGL will be an option at some point.  DirectX isn't officially
supported because it only works on Windows.)

