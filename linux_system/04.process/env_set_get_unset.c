#include<stdio.h>
#include<stdlib.h>

int main(){

    char env_name[15];
    char env_value[255];
    int overwrite=1;
    printf("Enter your Variable name:");
    scanf("%s", env_name);
    printf("Enter the Variable Value: ");
    scanf("%s", env_value);
    
    int status = setenv(env_name, env_value, overwrite);

    if(status == 0){
        printf("Environment variable Created Successfully.!\n");
    }else if(status == -1){
        printf("Environment variable creation failed.!\n");
    }
	printf("%s=%s\n",env_name,  getenv(env_name));
	status = unsetenv(env_name);
	printf("%s=%s\n",env_name,  getenv(env_name));
    return 0;
}
