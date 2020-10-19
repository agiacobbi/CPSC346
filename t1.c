int main() { 
    int a, b = 2, 3;
if (fork() == 0) { 
printf("a"); 
} 
else { 
printf("b"); 
} 
printf("c"); 
exit(0); 
}
