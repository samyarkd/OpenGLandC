#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_shader_content(const char *fileName) {
    FILE *fp;
    long size = 0;
    char *shaderContent;
    
    /* Read File to get size */
    fp = fopen(fileName, "rb");
    if (fp == NULL) {
        printf("Failed to open shader file: %s\n", fileName);
        perror("Error");  // This will print the system error message
        return "";
    }
    
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp) + 1;
    if (size <= 1) {
        printf("Shader file is empty: %s\n", fileName);
        fclose(fp);
        return "";
    }
    
    /* Reset file position and read content */
    fseek(fp, 0L, SEEK_SET);
    shaderContent = malloc(size);
    if (shaderContent == NULL) {
        printf("Failed to allocate memory for shader content\n");
        fclose(fp);
        return "";
    }
    
    memset(shaderContent, '\0', size);
    size_t read_size = fread(shaderContent, 1, size - 1, fp);
    if (read_size == 0) {
        printf("Failed to read shader file: %s\n", fileName);
        free(shaderContent);
        fclose(fp);
        return "";
    }
    
    fclose(fp);
    return shaderContent;
}
