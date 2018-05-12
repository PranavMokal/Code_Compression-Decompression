# Code_Compression-Decompression
Implementation of code compression and decompression using C++.
Code Format - 32 bit binary
Dictionary(static) - 8 entries 

/********************************************************************************/

Code compressed in the following seven possible formats:-

1.Format of the Run Length Encoding (RLE)
|000|Run Length Encoding(2 bits)|	

2.Format of bitmask-based compression – starting location is counted from left/MSB
|001|Starting Location(5 bits)|Bitmask(4 bits)|Dictionary Index (3 bits)|

3.Format of the 1 bit Mismatch – mismatch location is counted from left/MSB
|010|Mismatch Location(5 bits)|Dictionary Index|(3 bits)|

4.Format of the 2 bit consecutive mismatches – starting location is counted from left/MSB
|011|Starting Location(5 bits)|Dictionary Index(3 bits)|

5.Format of the 2 bit mismatches anywhere – Mismatch locations (ML) are counted from left/MSB
|100|1st ML from left(5 bits)|2nd ML from left(5 bits)|Dictionary Index (3 bits)|

6.Format of the Direct Matching		
|101|Dictionary Index(3 bits)|		

7.Format of the Original Binaries		
|110|Original Binary(32 bits)|	

/******************************************************************************/

File Formats

Compression:
Input : text file with 32-bit binaries
Output : 
A text file containing Compressed code appended together (32 bits in each line,last line padded with '1s' if needed).
a seperation marker "xxxx", followed by eight dictionaries

(For Decompression : Inverse of Compression)

/*********************************************************************************/

Command line parameters : "1" for compression . "2" for decompression.
