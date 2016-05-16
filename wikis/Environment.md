## Implementation Considerations

We had access to a server at edu.tyob.info. We chose to use this as both a build server and repository server.

## Hardware

* MSI Wind Nettop 100
* 1.6 GHz Intel Atom Dual Core N330 Processor
* 2GB DDR2 RAM (200-Pin SODIMM Laptop Memory)
* 2 TB SATA hard drive (Samsung Spinpoint F4 EcoGreen 2TB Serial ATA / SATA 3.0Gbps 3.5 Inch Hard Drive, 5400RPM)

## Server Environment

* OS: Debian GNU/Linux
* OS Version: Squeeze (Debian 6.0)

## Software

* Compiler: g++ (Debian 4.4.5-8) 4.4.5
* Library: GNU C Library (Debian EGLIBC 2.11.2-10) stable release version 2.11.2

## Version Control

* We chose to use git for version control
* Additional tools: gitosis for repository management over SSH & viewgit for a web based repository front-end

##Consequences

Our team was the only team to host their own repository & build server. 
Other teams did their development work using a Google code repository. 
There are pluses and minuses to both approaches. 

* The centralized build server eliminated the whole set of problems of 
compiling the code on different OS's, with different IDE's and with 
different libraries. Everyone was on the same page from day 1. 
Everyone could get the program to build. Since everyone was 
building and testing, check-ins tended not to destabilize the code. 
These factors helped us get to a working prototype faster. 

* viewgit did provide us with a nice web front-end comparable to that 
offered by Google code. However, we had no equivalent of the wiki that 
comes bundled with Google code projects. This deficiency became 
problematic for us later in the project.

