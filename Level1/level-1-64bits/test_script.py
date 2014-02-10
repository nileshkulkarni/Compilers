import os  
path = "test_files"
files=[]
for f in os.listdir(path):
    files.append(f) 

correct_files=[]
for f in files:
    fileName,fileExt = os.path.splitext( path + f)
    if( fileExt == '.c'):
        correct_files.append(f)



cfgFiles=[]
for f in correct_files:
    command = "make -f Makefile.cfg FILE=" + f+ " >/dev/null";
    os.system(command);
    cfgFiles.append(path+"/"+f+"s306.cfg"); 

print("Done generating cfg files\n");

for f in cfgFiles:
    print("Testing file  " + f ); 
<<<<<<< HEAD
    command = "./cfglp_original -ast -d -eval " + f + " > expected " 
    os.system(command) 
    command = "./cfglp -ast -d -eval " + f + " > generated " 
=======
    command = "./cfglpOriginal -eval -d " + f + " > expected " 
    os.system(command) 
    command = "./cfglp -eval -d " + f + " > generated " 
>>>>>>> f2c42704c73442e2a13fe8a93a194696b6e53aa8
    os.system(command)
    os.system("diff -b -B expected generated");


error_files=[]
for f in files:
    fileName,fileExt = os.path.splitext( path + f)
    if( fileExt == '.ecfg'):
        error_files.append(path + "/" +f)

for f in error_files:
    print("Testing file  " + f ); 
    command = "./cfglp64 -ast  -d " + f + " 2> expected " 
    os.system(command) 
    command = "./cfglp -ast -d " + f + " 2> generated " 
    os.system(command)
    #print("diff starts here \n");
    os.system("diff -b -B expected generated");



#print cfgFiles

