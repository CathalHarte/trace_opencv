Trace OpenCV
=============
The aim of this project is to provide a framework for adding traceability to a complicated set of image processing steps in OpenCV. A processing step is defined as any transform of: image -> image; image -> feature(s); feature(s) -> feature(s). A processing step will be considered "traceable" as long as the link between input and output is recorded, and can be visualised in the form of an image.

Support for tracing across frames of a video, and tracing where a processing step takes multiple inputs of images and features may be considered in the future.