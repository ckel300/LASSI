#include "global.h"
#include "parsekey.h"
#include "cluster.h"
#include "initialize.h"

int str2farr(char *strRaw, float fArray[MAX_AA]);

/// Parse_KeyFile - reads the parameter keyfile. I recommend using the Python scripts to generate key files that
/// are used in actual runs.
/// Have to painstakingly go through every different keyword that exists in the keyfile. Everytime you want to add something
/// to the keyfile, do it here and also the python scripts so that everything is consistent.
/// \param filename
/// \return
int Parse_Keyfile(char *filename) {
    FILE *infile;
    infile = fopen(filename, "r");
    int i;
    char strLine[1000];
    char strKeyword[1000];
    char strTemp[1000];
    char strEnergyFile[1000];
    char strStructFile[1000];
    int nStructFiletype = 0;
    char strTempArr[3][1000];
    strEnergyFile[0] = '\0';
    strStructFile[0] = '\0';

    int nLine = 0;

    int nErr = 0; // error code

    for (i = 0; i < MAX_MV; i++) {
        fMCFreq[i] = 0.0; // initialization; to be normalized //0 is initialized to 0.0
    }
    nThermalization_Mode = 0;
    Temp_Mode = -1;
    while (fgets(strLine, sizeof(strLine), infile) != NULL) {
        nLine++;

        strKeyword[0] = '#';
        sscanf(strLine, "%s", strKeyword);

        if (strKeyword[0] == '#') { // if the first character is #,
            // ignore the line
        } else {
            for (i = 0; strLine[i] != '\0'; i++) {
                if (strLine[i] == '#') {
                    strLine[i] = '\0'; // ignore the content after #
                    break;
                }
            }

            if (strcmp(strKeyword, "BOX_SIZE") == 0) {
                sscanf(strLine, "%*s %s %s %s", strTempArr[0], strTempArr[1], strTempArr[2]);
                if (strTempArr[1][0] < '0' || strTempArr[1][0] > '9') {
                    nBoxSize[0] = atoi(strTempArr[0]);
                    nBoxSize[1] = nBoxSize[0];
                    nBoxSize[2] = nBoxSize[0];
                } else {
                    if (strTempArr[2][0] < '0' || strTempArr[2][0] > '9') {
                        nBoxSize[0] = atoi(strTempArr[0]);
                        nBoxSize[1] = atoi(strTempArr[1]);
                        nBoxSize[2] = 0; // error handling required
                    } else {
                        nBoxSize[0] = atoi(strTempArr[0]);
                        nBoxSize[1] = atoi(strTempArr[1]);
                        nBoxSize[2] = atoi(strTempArr[2]);
                    }
                }
                nRDF_TotBins = nBoxSize[0] * 4;
                //nRDF_TotBins   = (lInt)sqrtf((float))
            } else if (strcmp(strKeyword, "MC_TEMP") == 0) {
                sscanf(strLine, "%*s %f", &fKT);
            } else if (strcmp(strKeyword, "N_STEPS") == 0) {
                sscanf(strLine, "%*s %ld", &nSteps);
            } else if (strcmp(strKeyword, "PREEQ_STEPS") == 0) {
                sscanf(strLine, "%*s %ld", &nPreSteps);
            } else if (strcmp(strKeyword, "PREEQ_TEMP") == 0) {
                sscanf(strLine, "%*s %f", &fPreKT);
            } else if (strcmp(strKeyword, "MC_TEMP_MODE") == 0) {
                sscanf(strLine, "%*s %d", &Temp_Mode);
            } else if (strcmp(strKeyword, "MC_DELTA_TEMP") == 0) {
                sscanf(strLine, "%*s %f", &fdelta_temp);
            } else if (strcmp(strKeyword, "MC_CYCLE_NUM") == 0) {
                sscanf(strLine, "%*s %d", &nTot_CycleNum);
            } else if (strcmp(strKeyword, "MC_INDENT_MODE") == 0) {
                sscanf(strLine, "%*s %d", &nThermalization_Mode);
            } else if (strcmp(strKeyword, "ROT_ENERGY_BIAS") == 0) {
                sscanf(strLine, "%*s %f", &f_globRotBias);
            } else if (strcmp(strKeyword, "MC_MAX_TRIALS") == 0) {
                sscanf(strLine, "%*s %d", &nMCMaxTrials);
            } else if (strcmp(strKeyword, "MV_TRANS_FREQ") == 0) {
                sscanf(strLine, "%*s %f", &fMCFreq[MV_TRANS]); //6
            } else if (strcmp(strKeyword, "MV_CLSTR_FREQ") == 0) {
                sscanf(strLine, "%*s %f", &fMCFreq[MV_CLSTR]); //8
            } else if (strcmp(strKeyword, "MV_SMCLSTR_FREQ") == 0) {
                sscanf(strLine, "%*s %f", &fMCFreq[MV_SMCLSTR]); //7
            } else if (strcmp(strKeyword, "MV_STROT_FREQ") == 0) {
                sscanf(strLine, "%*s %f", &fMCFreq[MV_STROT]); //1
            } else if (strcmp(strKeyword, "MV_LOCAL_FREQ") == 0) {
                sscanf(strLine, "%*s %f", &fMCFreq[MV_LOCAL]); //2
            } else if (strcmp(strKeyword, "MV_SNAKE_FREQ") == 0) {
                sscanf(strLine, "%*s %f", &fMCFreq[MV_SNAKE]); //5
            } else if (strcmp(strKeyword, "MV_DBPVT_FREQ") == 0) {
                sscanf(strLine, "%*s %f", &fMCFreq[MV_DBPVT]); //11
            } else if (strcmp(strKeyword, "MV_COLOCAL_FREQ") == 0) {
                sscanf(strLine, "%*s %f", &fMCFreq[MV_COLOCAL]); //3
            } else if (strcmp(strKeyword, "MV_MTLOCAL_FREQ") == 0) {
                sscanf(strLine, "%*s %f", &fMCFreq[MV_MTLOCAL]); //4
            } else if (strcmp(strKeyword, "MV_PIVOT_FREQ") == 0) {
                sscanf(strLine, "%*s %f", &fMCFreq[MV_PIVOT]); //9
            } else if (strcmp(strKeyword, "MV_BRROT_FREQ") == 0) {
                sscanf(strLine, "%*s %f", &fMCFreq[MV_BRROT]); //10
            } else if (strcmp(strKeyword, "RESTART_FILE") == 0) {
                sscanf(strLine, "%*s %s", strRestartFile);
            } else if (strcmp(strKeyword, "STRUCT_FILETYPE") == 0) {
                sscanf(strLine, "%*s %d", &nStructFiletype);
            } else if (strcmp(strKeyword, "STRUCT_FILE") == 0) {
                sscanf(strLine, "%*s %s", strStructFile);
            } else if (strcmp(strKeyword, "ENERGY_FILE") == 0) {
                sscanf(strLine, "%*s %s", strEnergyFile);
            } else if (strcmp(strKeyword, "RANDOM_SEED") == 0) {
                sscanf(strLine, "%*s %d", &RNG_Seed);
                RNG_Seed = RNG_Seed == 0 ? time(NULL) : RNG_Seed;
            } else if (strcmp(strKeyword, "REPORT_PREFIX") == 0) {
                sscanf(strLine, "%*s %s", strReportPrefix);
            } else if (strcmp(strKeyword, "REPORT_LOG_FREQ") == 0) {
                sscanf(strLine, "%*s %ld", &nReport[REPORT_LOG]);
            } else if (strcmp(strKeyword, "REPORT_ENERGY_FREQ") == 0) {
                sscanf(strLine, "%*s %ld", &nReport[REPORT_ENERGY]);
            } else if (strcmp(strKeyword, "REPORT_CONFIG_FREQ") == 0) {
                sscanf(strLine, "%*s %ld", &nReport[REPORT_CONFIG]);
            } else if (strcmp(strKeyword, "REPORT_MCMOVE_FREQ") == 0) {
                sscanf(strLine, "%*s %ld", &nReport[REPORT_MCMOVE]);
            } else if (strcmp(strKeyword, "REPORT_NETWORK_FREQ") == 0) {
                sscanf(strLine, "%*s %ld", &nReport[REPORT_NETWORK]);
            } else if (strcmp(strKeyword, "REPORT_RDFTOT_FREQ") == 0) {
                sscanf(strLine, "%*s %ld", &nReport[REPORT_RDFTOT]);
            } else {
                fprintf(stderr, "ERROR: unable to parse line %d in %s.\n%s", nLine, filename, strLine);
                exit(1);
            }
        }
    }


    fclose(infile);


    float freq_tot = 0.0;
    for (i = MV_NULL + 1; i < MAX_MV; i++) {
        freq_tot += fMCFreq[i];
    }

    if (freq_tot == 0) {
        for (i = MV_NULL + 1; i < MAX_MV; i++) {
            fMCFreq[i] = 1.0 / (float) (MAX_MV - 1);
        }
    } else {
        for (i = MV_NULL + 1; i < MAX_MV; i++) {
            fMCFreq[i] /= freq_tot;
        }
    }

    if (strEnergyFile[0] != '\0') {
        nErr = Parse_EnergyFile(strEnergyFile);
    }

    if (strStructFile[0] != '\0') {
        Parse_StructureFile(strStructFile);
        if (nStructFiletype == 0) {
            bReadConf = 0;
        } else if (nStructFiletype == 1) {
            printf("Reading restart file provided to generate initial configuration.\n");
            bReadConf = 1;
        } else {
            fprintf(stderr, "ERROR: undefined value in STRUCT_FILETYPE of %s.\nCrashing!\n", filename);
            exit(1);
        }
    } else {
        bReadConf = -1;
    }
    if (bReadConf == 1) {
        if (strRestartFile[0] != '\0') {
            printf("Restart file is %s\n", strRestartFile);
        } else {
            nErr = 5;
            printf("No restart file provided.\n");
        }
        //exit(1);
    }

    return nErr;
}

/// Parse_EnergyFile - reads the energy file
/// Have to painstakingly go through every different keyword that exists in the energy file. Reads all the matrices
/// \param strEnFile
/// \return
int Parse_EnergyFile(char *strEnFile) {
    int nRes = 0;

    FILE *infile;
    infile = fopen(strEnFile, "r");

    char strLine[100];
    int nFlag = 0;
    char bOrder = 0;
    char strKey[100];
    int nRow;
    float fTemp[MAX_AA];
    int nTemp[MAX_AA];
    int i, j, k;
    int nEntry;
    char bEnergy[MAX_E], bRad[MAX_E];
    for (i = 0; i < MAX_E; i++) { // initialization
        bEnergy[i] = 0;
        bRad[i] = 0;
    }

    while (fgets(strLine, sizeof(strLine), infile) != NULL) {

        if (strLine[0] == '#') {
            sscanf(&strLine[1], "%s", strKey);

            if (strcmp(strKey, "STICKERS") == 0) nFlag = -1;
            else if (bOrder == 0) {
                fprintf(stderr, "ERROR: #STICKERS is not the first entry in %s.\n", strEnFile);
                nRes = 1;
                break;
            } else {
                if (strcmp(strKey, "OVERLAP_POT") == 0) {
                    nFlag = 2 * (E_OVLP);
                    bEnergy[E_OVLP] = 1;
                } else if (strcmp(strKey, "CONTACT_POT") == 0) {
                    nFlag = 2 * (E_CONT);
                    bEnergy[E_CONT] = 1;
                } else if (strcmp(strKey, "CONTACT_RAD") == 0) {
                    nFlag = 2 * (E_CONT) + 1;
                    bRad[E_CONT] = 1;
                } else if (strcmp(strKey, "SC_SC_POT") == 0) {
                    nFlag = 2 * (E_SC_SC);
                    bEnergy[E_SC_SC] = 1;
                } else if (strcmp(strKey, "LINKER_LENGTH") == 0) {
                    nFlag = -3;
                } else if (strcmp(strKey, "LINKER_SPRCON") == 0) {
                    nFlag = -4;
                } else if (strcmp(strKey, "LINKER_EQLEN") == 0) {
                    nFlag = -5;
                } else {
                    fprintf(stderr, "ERROR: irregular expression in %s: %s\n", strEnFile, strKey);
                    nRes = 2;
                    break;
                }
            }

            nRow = 0;
        } else if (strcmp(strLine, "\n") != 0) { // ignore empty lines
            if (nFlag == -1) { // sticker
                sscanf(strLine, "%d", &nBeadTypes);
                if (nBeadTypes > MAX_AA) {
                    fprintf(stderr, "ERROR: the number of AA types exceeds MAX_AA in %s.\n", strEnFile);
                    nRes = 3;
                    break;
                }
                bOrder = 1;
            } else if (nFlag == -3) { // linker_length
                sscanf(strLine, "%f", &fLinkerLength);
            } else if (nFlag == -4) { // linker_sprcon
                sscanf(strLine, "%f", &fLinkerSprCon);
            } else if (nFlag == -5) { // linker_eqlen
                sscanf(strLine, "%f", &fLinkerEqLen);
            } else if (nFlag == 0) {
                fprintf(stderr, "ERROR: nFlag is not assigned in %s.\n", strEnFile);
                nRes = 4;
                break;
            } else {
                nEntry = str2farr(strLine, fTemp);
                printf("nEntry %d\n", nEntry);
                printf("nBeadTypes %d\n", nBeadTypes);

                if (nEntry != nBeadTypes) {
                    if (nEntry == 1) {
                        for (i = 0; i < nBeadTypes; i++) {
                            for (j = 0; j < nBeadTypes; j++) {
                                if (nFlag % 2 == 0) fEnergy[i][j][(int) (nFlag / 2)] = fTemp[0];
                                else fEnRad[i][j][(int) (nFlag / 2)] = fTemp[0];
                            }
                        }
                    } else {
                        fprintf(stderr, "ERROR: irregular expression in energy matrices of %s.\n", strEnFile);
                        nRes = 4;
                        break;
                    }
                } else {
                    if (nFlag % 2 == 0) { // energy
                        for (i = 0; i < nBeadTypes; i++) fEnergy[nRow][i][(int) (nFlag / 2)] = fTemp[i];
                    } else { // radius
                        for (i = 0; i < nBeadTypes; i++) fEnRad[nRow][i][(int) (nFlag / 2)] = fTemp[i];
                    }
                    nRow++;
                }
            }
        }
    }

    fclose(infile);

    return nRes;
}

/// str2farr - converts the string array to an array of floats. The nice thing about C is the ease of string manipulation.
/// used to generate matrices from the energy file.
/// \param strRaw
/// \param fArray
/// \return Changes fArray to include be the array in
int str2farr(char strRaw[], float fArray[MAX_AA]) {
    int i;
    char *strTemp;
    char *token;
    char *saveptr;

    strTemp = strRaw;
    token = "";

    char *deli = "  \t"; // delimeter

    for (i = 0; i < MAX_AA; i++, strTemp = NULL) {
        token = strtok_r(strTemp, deli, &saveptr);

        if (token == NULL) break;
        fArray[i] = atof(token);
    }

    return i;
}

/// Parse_StructureFile - reads the structure file filename.
/// The format of the structure file is in the function below. Should be easier to use Python to generate the files,
/// usually.
/// \param filename
void Parse_StructureFile(char *filename) {
    /*
    This function reads in a structure file that also includes topology information. The format is:
    # The '#' is the commenting character.
    NEW{
    nCopiesOfMolecule
    #AtomID AtomType LinkerLengths BondedPartner
    }END
    Each molecule is within NEW{\0 ... \0}END which act as keywords. After NEW{, the next line contains the number of copies to be made of this molecule. It is also expected that whenever a bead is listed, ALL of its bonds follow in the next lines.
    E.G A molecule with 5 branches where each branch has 2 beads would be:
    ###
    NEW{
    nCopies
    0 0 1 1
    0 0 1 2
    0 0 1 3
    0 0 1 4
    0 0 1 5
    1 1 1 0
    1 1 1 6
    2 1 1 0
    2 1 1 7
    3 1 1 0
    3 1 1 8
    4 1 1 0
    4 1 1 9
    5 1 1 0
    5 1 1 10
    6 2 1 1
    7 2 1 2
    8 2 1 3
    9 2 1 4
    10 2 1 5
    }END
    ####
    As such, each molecule is independent and all molecule beads start from 0!
    For each listed bead, linker_len[i][j] corresponds to the linker constraint between bead i, and
    bead (topo_info[i][j]). After each molecule is read, nCopies copies are made, and THEN, the next line in the file is read.
    */
    FILE *inFile;
    inFile = fopen(filename, "r");//Opening the file!

    char strLine[1000];//Used to store each line from the input file.
    char strKeyword[1000];//Used to convert strLine into specific keywords.
    int curID;//Used to track the current beadID
    int curType, curLinker, curPartner;//Used to track current bead-type,linker-length(s),bond-partner.
    int xval, yval, zval;
    int nCopies;//Used to store how many copies to make.
    int nChainStart;//Tracking which beadID each chain starts from.
    int i, j, k;//Just some iterators for looping
    int nCursor, nCursor2, nTemp; //Internal iterators to make sure beads and their partners are not double counted.
    int nChainID, nChainType;//Internal iterator to count which chainID the chain is.
    int nFlag;//Internal flag used to see which keyword is being read!
    int nBEADS;//Internal counter to count unique beads in each chain!
    //Initialize the bead_info and chain_info
    for (i = 0; i < MAX_BEADS; i++) {
        for (j = 0; j < BEADINFO_MAX; j++) {
            bead_info[i][j] = -1;
        }
        for (j = 0; j < MAX_BONDS; j++) {
            topo_info[i][j] = -1;
            linker_len[i][j] = -1;
        }
    }
    for (i = 0; i < MAX_CHAINS; i++) {
        for (j = 0; j < CHAININFO_MAX; j++) {
            chain_info[i][j] = -1;
        }
    }

    for (i = 0; i < MAX_CHAINTYPES; i++) {
        for (j = 0; j < MAX_CHAINLEN; j++) {
            for (k = 0; k < POS_MAX2; k++) {
                chain_weight[i][j][POS_MAX] = 0;
            }
        }
    }

    //Initialization of the counters and iterators
    nCursor = -1;
    nCursor2 = -1;
    int nCursorHold = 0;
    nChainID = -1;
    nChainType = -1;
    nFlag = -1;
    nTemp = -1;
    tot_beads = 0;
    tot_chains = 0;
    nBEADS = 0;
    int beadLoc = 0;
    int beadLocHold = 0;
    nChainStart = 0;
    nCopies = 0;
    int Partition = 0;
    xval = 0;
    yval = 0;
    zval = 0;
    int curIDsave = 0;

    while (fgets(strLine, sizeof(strLine), inFile) != NULL && nFlag == -1) {
        //Keep reading the file until it ends or it's incorrectly formatted.

        strKeyword[0] = '#';//This is the commenting character in all input files.
        sscanf(strLine, "%s", strKeyword);//Plain old read the line.

        if (strKeyword[0] != '#') {//If the line is not a comment, see what keyword it is
            for (i = 0; strLine[i] != '\0'; i++) {//Keep reading till end of that line
                if (strLine[i] == '#') {//This line now has a comment, so stop reading further
                    strLine[i] = '\0';//Just assume we have reached the end of the line
                    break;
                }
            }
            if (strcmp(strKeyword, "NEW{") == 0) {//This signifies a new molecule type has been started
                nFlag = 1;
            }
            if (strcmp(strKeyword, "}END") == 0) {//This signifies a new molecule type has been started
                nFlag = -1;
            }
        }

        if (nFlag == 1) {//This signifies that a new molecule has started
            //It's assumed that the next line contains the number of copies for this molecule.
            Partition = 0;
            beadLoc = 0;
            nChainType++;
            nChainTypeIsLinear[nChainType] = 1;//Assume all chains are linear to begin with.
            nChainStart += nBEADS;
            printf("CHAIN START NOW IS %d\n", nChainStart);
            nChainID++;
            tot_chains++;
            nBEADS = 0;
            fgets(strLine, sizeof(strLine), inFile);//Reading the next line, which has nMOLS
            sscanf(strLine, "%d", &nCopies);//Remembering how many copies top make.
            printf("NUMBER OF COPIES IS %d\n", nCopies);
            while (fgets(strLine, sizeof(strLine), inFile) != NULL && nFlag == 1) {
                sscanf(strLine, "%s", strKeyword);//Just reading as a string
                if (strcmp(strKeyword, "}END") == 0) {//End of this molecule
                    nFlag = -1;
                    printf("CUR ID IS %d\n", curIDsave);
                    break;
                }
                sscanf(strLine, "%d %d %d %d %d %d %d", &curID, &curType, &curLinker, &curPartner, &xval, &yval, &zval);
                curIDsave = curID;
                curID += nChainStart;//Accounting for previously defined beads
                if (curIDsave == 0) {
                   printf("SO THE VAL HERE IS %d\n", bead_info[curID][BEAD_TYPE]);
                   printf("THE ID HERE IS %d\n", curID);
                }
                beadLocHold = beadLoc;
                if (bead_info[curID][BEAD_TYPE] ==
                    -1) {//This is to make sure that each bead is counted once only even if it has many bonds.
                    if (curIDsave == 0) {
                        printf("FOUND ONE");
                    }
                    tot_beads++;
                    nBEADS++;
                    printf("TOTAL BEADS NOW IS %d\n and currentid is %d\n", tot_beads,curIDsave);
                    bead_info[curID][BEAD_TYPE] = curType;
                    bead_info[curID][BEAD_CHAINID] = nChainID;
                    if (curID != 0) {
                        nCursorHold = nCursor;
                    }
                    nCursor = 0;//This is a counter for number of bonds, which should reset when you have a 'new' bead.

                    chain_weight[nChainType][beadLoc][POS_X] = xval;
                    chain_weight[nChainType][beadLoc][POS_Y] = yval;
                    chain_weight[nChainType][beadLoc][POS_Z] = zval;
                    beadLoc++;
                }
                if (curPartner != -1) {//This bead has a bonded partner
                    if (nCursor >
                        1) {//This signifies that the chain is not linear because a bead has more than two bonds because indecies start at 0
                        nChainTypeIsLinear[nChainType] = 0;
                    }
                    curPartner += nChainStart;//Accounts for all beads before, like above.
                    topo_info[curID][nCursor] = curPartner;//Adding the ID of the partner
                    linker_len[curID][nCursor] = curLinker * (int) fLinkerLength;//Adding the linker constraint.
                    nCursor++;

                }
                if (beadLocHold != beadLoc) {  //this checks to see if we are still looking at the same bead here; if not, we use the vals of the previous beads bond number and its location in the chain; could have used "nBEADS" but wanted to be safe for now
                    chain_weight[nChainType][beadLocHold][POS_MAX] = nCursorHold;
                }

            }
            for (i = 0; i < nBEADS; i++) {
                Partition += chain_weight[nChainType][i][POS_MAX]; //calculate the partition function
            }
            for (i = 0; i < nBEADS; i++) {
                chain_weight[nChainType][i][POS_MAX] /= Partition; //calculate normalized weights
            }
            for (i = 1; i < nBEADS; i++) {
                chain_weight[nChainType][i][POS_MAX] += chain_weight[nChainType][i-1][POS_MAX]; //calculate cumulative
            }
            chain_info[nChainID][CHAIN_START] = nChainStart;
            chain_info[nChainID][CHAIN_LENGTH] = nBEADS;
            chain_info[nChainID][CHAIN_TYPE] = nChainType;
            //We just fully store the first chain 'manually'. Now we just copy the chain nCopies times.
            for (k = 1; k < nCopies; k++) {//Now we just copy this molecule nMOL-1 times
                printf("doing it\n");
                nChainStart += nBEADS;//This accounts for chain lengths.
                nChainID++;//Going to the next chainID
                tot_chains++;//Add a chain
                chain_info[nChainID][CHAIN_START] = nChainStart;
                chain_info[nChainID][CHAIN_LENGTH] = nBEADS;
                chain_info[nChainID][CHAIN_TYPE] = nChainType;
                for (i = 0; i < nBEADS; i++) {
                    tot_beads++;
                    curID = i + nChainStart;
                    nTemp = curID - nBEADS;
                    bead_info[curID][BEAD_TYPE] = bead_info[nTemp][BEAD_TYPE];
                    bead_info[curID][BEAD_CHAINID] = nChainID;
                    for (j = 0; j < MAX_BONDS; j++) {
                        linker_len[curID][j] = linker_len[nTemp][j];
                        if (topo_info[nTemp][j] != -1) {
                            topo_info[curID][j] =
                                    topo_info[nTemp][j] + nBEADS;//Because we must account for chain lengths
                        }
                    }
                }
            }
            nFlag = -1;//Reseting the flag.
        }

        //printf("Done with structure!\n");
        //exit(1);
        //This is to make sure that the reading was done correctly.
        if (nFlag != -1) {
            printf("Incorrectly formatted input structure file. I must crash :(\n\n");
            exit(1);
        }
    }
    fclose(inFile);

}

