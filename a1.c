#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CS265 Assignment 1 - Jack Pinkstone

// Functions written after main()
void trim_str(char *str);
int round_tax(float num);

int main(int argc, char *argv[]) {

    // Variables to be used throughout the program
    char *order_file, *tax_rate_file, *invoice_file, *token, *tax_token, *save_ptr1, *save_ptr2;
    char order_line[256], tax_rate_line[256], product[100], state[100];
    int quantity = 0, price = 0, tax = 0, total = 0, total_tax = 0, grand_total = 0;

    // Check the command line arguments and set filenames
    if(argc == 4){
        order_file = argv[1];
        tax_rate_file = argv[2];
        invoice_file = argv[3];
    }else if(argc != 1){
        printf("The command line arguments are incorrect\n");
        return -1;
    }else{
        order_file = "order.txt";
        tax_rate_file = "tax_rate.txt";
        invoice_file = "invoice.txt";
    }

    // Open the 3 files
    FILE *order_file_content = fopen(order_file, "r");
    FILE *tax_rate_file_content = fopen(tax_rate_file, "r");
    FILE *invoice_file_content = fopen(invoice_file, "w");
    
    // If the files cannot be opened, exit the program
    if(!order_file_content || !tax_rate_file_content || !invoice_file_content){
        printf("Cannot open one or more of the files\n");
        return -1;
    }
    
    // Add the headers to the invoice file
    fprintf(invoice_file_content, "%s%20s%13s%10s%10s%15s\n", "Product", "Price", "Quantity", "State", "Tax", "Subtotal");
    fprintf(invoice_file_content, "---------------------------------------------------------------------------\n");

    // Looping through each line of the order file
    while(fgets(order_line, 256, order_file_content)){
        token = strtok_r(order_line, ",", &save_ptr1);
        // Looping through each token in the line
        for(int i = 0; token != NULL; i++){
            trim_str(token);    // Trim leading and trailing spaces
            // Set the product name, quantity, price, and state to variables
            if(i == 0){
                strcpy(product, token);
            }else if(i == 1){
                quantity = atoi(token);
            }else if(i == 2){
                price = (int) (atof(token)*100.0f); // Storing money as cents
            }else{
                strcpy(state, token);
            }
            // Looping through the tax rates file to find the correct tax rate for the product
            while(fgets(tax_rate_line, 256, tax_rate_file_content)){
                tax_token = strtok_r(tax_rate_line, ",", &save_ptr2);
                while(tax_token != NULL){
                    // When the product state is in the current line
                    if(strcmp(token, tax_token) == 0){
                        tax_token = strtok_r(NULL, ",", &save_ptr2);
                        trim_str(tax_token);
                        // Calculate the tax and subtotal
                        tax = round_tax((atof(tax_token)/100)*(price*quantity));
                        total = (price*quantity)+tax;
                        // Add the total tax and grand total for later
                        total_tax = total_tax + tax;
                        grand_total = grand_total + total;
                        // Add the product line to the invoice with calculated values
                        fprintf(invoice_file_content, "%-15s%12.2f%13d%10s%10.2f%15.2f\n", product, ((float)price)/100.0f, quantity, state, ((float)tax)/100.0f, ((float)total)/100.0f);
                    }
                    tax_token = strtok_r(NULL, ",", &save_ptr2);
                }
            }
            rewind(tax_rate_file_content);  // Rewind the file to read from the beginning
            token = strtok_r(NULL, ",", &save_ptr1);
        }
    }

    // Add the total tax and grand total to the end of the invoice file
    fprintf(invoice_file_content, "Total Tax Due: %.2f\n", ((float)total_tax)/100.0f);
    fprintf(invoice_file_content, "Grand Total: %.2f\n", ((float)grand_total)/100.0f);
    
    // Close all the files
    fclose(order_file_content);
    fclose(tax_rate_file_content);
    fclose(invoice_file_content);

}

// Function to trim the leading and trailing spaces from a string
void trim_str(char *str){
    int index = 0, i = 0;

    // Find where to remove leading spaces up to
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n'){
        index++;
    }

    // Move trailing characters to the left
    while(str[i + index] != '\0'){
        str[i] = str[i + index];
        i++;
    }

    str[i] = '\0';

    // Remove trailing spaces
    i = 0;
    index = -1;
    while(str[i] != '\0'){
        if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n'){
            index = i;
        }
        i++;
    }

    str[index + 1] = '\0';
}

// Function to round the tax so it can be stored correctly as cents
int round_tax(float num){
    //Find the digit after the decimal that will determine the rounding
    int afterDecimal = (int)((num-(int)num)*10);

    // If the tens place is 5 or above, add 1
    if(afterDecimal >= 5){
        return ((int)num)+1;
    }else{
        return (int)num;
    }
}