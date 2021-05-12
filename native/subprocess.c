#include "StrChain.h"
#include "subprocess.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
ValeStr* ValeStrNew(int64_t length);

long read_into_buffer(char* buffer, ValeInt bytes, FILE* stream){
    long i = 0;
    for(i = 0; i < bytes; i++){
        if(feof(stream)) {
            break;
        }
        buffer[i] = fgetc(stream);
    }
    return i;
}

/*
   The latest version of this library is available on GitHub;
   https://github.com/sheredom/subprocess.h
*/

ValeStr* get_env_var(ValeStr* var_name) {
    const char* env_var = getenv((const char*)&var_name->chars);
    unsigned long length = strlen(env_var);
    ValeStr* out = malloc(sizeof(ValeStr) + length + 1);
    out->length = length; 
    strcpy(out->chars, env_var);
    return out;
}

char** vale_to_char_arr(StrChain* chains) {
    char** args = malloc(chains->length*sizeof(char*)+sizeof(char*));
    for(unsigned long i = 0; i < chains->length; i++) {
        args[i] = &chains->elements[i]->chars[0];
    }
    args[chains->length] = 0;
    return args; 
}

ValeInt launch_command(StrChain* chain) {
    ValeInt out = 0;
    const char** args = (const char**)vale_to_char_arr(chain);
    struct subprocess_s* subproc = malloc(sizeof(struct subprocess_s));
    if(subprocess_create(args, subprocess_option_inherit_environment, subproc) != 0){
        perror("command creation failed");
    }
    out = (unsigned long)subproc;
    free(args);
    ValeReleaseMessage(chain); 
    return out;
}

ValeStr* read_child_stdout(ValeInt cmd, long bytes) {
    ValeStr* out = ValeStrNew(bytes+1); 
    FILE* stdout_handle = subprocess_stdout((struct subprocess_s*)cmd); 
    long read_len = read_into_buffer(out->chars, bytes, stdout_handle);
    out->chars[bytes] = '\0'; 
    out->length = read_len;
    fclose(stdout_handle);
    return out;
}

ValeStr* read_child_stderr(ValeInt cmd, long bytes) {
    ValeStr* out = ValeStrNew(bytes+1);
    FILE* stderr_handle = subprocess_stderr((struct subprocess_s*)cmd); 
    long read_len = read_into_buffer(out->chars, bytes, stderr_handle);
    out->chars[bytes] = '\0'; 
    out->length = read_len;
    fclose(stderr_handle);
    return out;
}

void write_child_stdin(ValeInt cmd, ValeStr* contents) {
    
}

long join_subprocess(long handle){
    int result;
    subprocess_join((struct subprocess_s*)handle, &result);
    return result;  
}
