# DrCodeCov
Code Coverage client for DynamoRIO

# About
Its slightly different from DynamoRIO's drcov in terms of data. DrCodeCov will create for each loaded module a bitmap that is the size of the entire image, it will mark each byte individually which can tell us following things:
- Is instruction start.
- Is instruction part.
- Is branch.
- Unreached.
DrCodeCov will not count hits, this is something I did not need. You can identify which code is executed or if code jumps into parts of instruction operands, this is common in obfuscated code.

# DrCov format.
DrCodeCov can output the same format as drcov, specify this via the client option "-format drcov". We however suggest using the binary format for extended details.

# Building
The project currently comes with a Visual Studio 2017 project. Make sure you have set environment variable DYNAMORIO_HOME to your DynamoRIO directory. Open the project in Visual Studio and choose your desired configuration to build. 
