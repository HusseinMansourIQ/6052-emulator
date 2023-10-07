
#include <iostream>
#include <cstdio>
#include <cstdlib>

using Byte = unsigned char;
using Word = unsigned short;
using u32 = unsigned int;

struct  Mem
{
    static constexpr u32 MAX_MEM = 1023 * 64;
    Byte Data[MAX_MEM];

    void Initialise() {
        for (u32 i = 0; i < MAX_MEM; i++) {
            Data[i] = 0;
        };
    };
    //reading one byte
    Byte operator[](u32 Adress)const {
        return Data[Adress];
    };

    //write one byte
    Byte& operator[](u32 Adress) {
        return Data[Adress];
    };

    //write tow bytes 
    void WriteWord(Word Value,u32& Cycles , u32 Address){
        Data[Address] = Value & 0xFF;
        Data[Address + 1] = (Value >> 8);
        Cycles -= 2;
        }
};
struct CPU{
    
    Word PC;
    Byte SP;
    Byte A, X, Y; //registers 
    // the A is accumulator , X,Y are index registers
    Byte C : 1; //status flag 
    Byte Z : 1;
    Byte I : 1;
    Byte D : 1;
    Byte B : 1;
    Byte V : 1;
    Byte N : 1;

    void Reset(Mem& memory) {
        PC = 0xfffC;
        SP = 0x0100;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        memory.Initialise();
    }

    Byte FetchByte(u32& Cycles, Mem& memory) {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;
    }

    Word FetchWord(u32& Cycles, Mem& memory) {
        Word Data = memory[PC];
        PC++;
        Data |= (memory[PC] << 8);
        PC--;
        Cycles -= 2;
        return Data;
    }

    Byte& ReadByte(u32& Cycles,Byte Address , Mem& memory) {
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    static constexpr Byte INS_LDA_ZP = 0xA5;
    static constexpr Byte INS_LDA_IM = 0xA9;
    static constexpr Byte INS_LDA_ZPX = 0xB5;
    static constexpr Byte INS_JSR = 0x20;

    void LDASetStatus() {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;

    }
    void Execute(u32 Cycles, Mem& memory) {
        while (Cycles >0)
        {
            Byte Ins = FetchByte(Cycles, memory);
            switch (Ins)
            {
            case INS_LDA_IM: {
                Byte Value = FetchByte(Cycles, memory) >0;
                A = Value;
                LDASetStatus();
            } break;
            case INS_LDA_ZP: {
                Byte ZeroPageAddress = FetchByte(Cycles, memory);
                A = ReadByte(Cycles, ZeroPageAddress, memory);
                LDASetStatus();

            }break;
            case INS_LDA_ZPX: {
                Byte ZeroPageAddress = FetchByte(Cycles, memory);
                ZeroPageAddress += X;
                Cycles--;
                A = ReadByte(Cycles, ZeroPageAddress, memory);
                LDASetStatus();

            }break;
            case INS_JSR: {
                Word SubAddr = FetchWord(Cycles, memory);
                memory.WriteWord(PC - 1, Cycles, SP);
                PC = SubAddr;
                SP++;
                Cycles--;
            }
            default: {
                std::cout << "inst not handled ";
            }
                break;
            }}}};
int main()
{
    Mem mem;
    CPU cpu;
    cpu.Reset(mem);
    //LOADED FROM DISCK
    mem[0xFFFC] = 0x20; // same INS_LDA_ZP
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x48;

    cpu.Execute(3, mem);
    return 0;

}
