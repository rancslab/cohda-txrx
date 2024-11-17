# Cohda-txrx

## Setup

Clone the project to the Cohda SDK VM. All distros of Linux have the necessary libraries, but the Cohda SDK comes loaded with all of the correct versions of those libraries, so it’s most convenient to clone it there. After this, open the directory created for the project as a workspace in Eclipse. Because you’ll have never opened this folder as a workspace before, none of the projects will load and you’ll have to tell eclipse to associate that workspace folder with the projects in it.

To do this:

- Click on “Import projects” in the project explorer pane
- Use the import wizard at “General/Existing project into workspace”
- Set root directory to the workspace folder
- Click finish

## Project details

Building with the debug configuration will produce an x86_64 executable meant to be tested on the Cohda SDK VM. Using the release configuration will produce an aarch64 executable that is capable of running on the Cohda itself.
