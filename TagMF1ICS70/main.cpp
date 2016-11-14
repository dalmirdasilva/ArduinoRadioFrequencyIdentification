#include <stdio.h>
#include "TagMF1ICS70.cpp"

int main() {

    TagMF1ICS70 tag;

    // getSize

    if (tag.getSize() !=  4096) {
        printf("error in getSize#0\n");
    }

    // getSectorCount

    if (tag.getSectorCount() != 40) {
        printf("error in getSectorCount#0\n");
    }

    // getBlockCount

    if (tag.getBlockCount() != 256) {
        printf("error in getBlockCount#0\n");
    }

    // getBlockCountInSector

    if (tag.getBlockCountInSector(0) != 4) {
        printf("error in getBlockCountInSector#0\n");
    }

    if (tag.getBlockCountInSector(32) != 16) {
        printf("error in getBlockCountInSector#1\n");
    }

    // getBlockCountInSector

    if (tag.getBlockCountInSector(0) != 4) {
        printf("error in getBlockCountInSector#0\n");
    }

    if (tag.getBlockCountInSector(31) != 4) {
        printf("error in getBlockCountInSector#1\n");
    }

    if (tag.getBlockCountInSector(32) != 16) {
        printf("error in getBlockCountInSector#2\n");
    }

    if (tag.getBlockCountInSector(40) != 16) {
        printf("error in getBlockCountInSector#3\n");
    }

    // getSectorSize

    if (tag.getSectorSize(0) != 4*16) {
        printf("error in getSectorSize#0\n");
    }

    if (tag.getSectorSize(31) != 4*16) {
        printf("error in getSectorSize#1\n");
    }

    if (tag.getSectorSize(32) != 16*16) {
        printf("error in getSectorSize#2\n");
    }

    if (tag.getSectorSize(40) != 16*16) {
        printf("error in getSectorSize#3\n");
    }

    // addressToSector

    if (tag.addressToSector(10) != 2) {
        printf("error in addressToSector#0\n");
    }

    if (tag.addressToSector(4) != 1) {
        printf("error in addressToSector#1\n");
    }

    if (tag.addressToSector(0) != 0) {
        printf("error in addressToSector#2\n");
    }

    if (tag.addressToSector(12) != 3) {
        printf("error in addressToSector#3\n");
    }

    if (tag.addressToSector(127) != 31) {
        printf("error in addressToSector#4\n");
    }

    if (tag.addressToSector(128) != 32) {
        printf("error in addressToSector#5\n");
    }

    if (tag.addressToSector(143) != 32) {
        printf("error in addressToSector#6\n");
    }

    if (tag.addressToSector(144) != 33) {
        printf("error in addressToSector#7\n");
    }

    if (tag.addressToSector(255) != 39) {
        printf("error in addressToSector#8\n");
    }

    // isAddressSectorTrailer

    if (tag.isAddressSectorTrailer(12)) {
        printf("error in isAddressSectorTrailer#0\n");
    }

    if (!tag.isAddressSectorTrailer(3)) {
        printf("error in isAddressSectorTrailer#1\n");
    }

    if (!tag.isAddressSectorTrailer(19)) {
        printf("error in isAddressSectorTrailer#2\n");
    }

    if (tag.isAddressSectorTrailer(22)) {
        printf("error in isAddressSectorTrailer#3\n");
    }

    if (!tag.isAddressSectorTrailer(127)) {
        printf("error in isAddressSectorTrailer#4\n");
    }

    if (!tag.isAddressSectorTrailer(255)) {
        printf("error in isAddressSectorTrailer#5\n");
    }

    if (tag.isAddressSectorTrailer(254-16)) {
        printf("error in isAddressSectorTrailer#6\n");
    }

    // getSectorTrailerAddress

    if (tag.getSectorTrailerAddress(31) != 127) {
        printf("error in getSectorTrailerAddress#4\n");
    }

    if (tag.getSectorTrailerAddress(32) != 143) {
        printf("error in getSectorTrailerAddress#5, got %d expect %d\n", tag.getSectorTrailerAddress(32), 143);
    }

    if (tag.getSectorTrailerAddress(39) != 255) {
        printf("error in getSectorTrailerAddress#6, got %d expect %d\n", tag.getSectorTrailerAddress(39), 255);
    }

    if (tag.getSectorTrailerAddress(1) != 7) {
        printf("error in getSectorTrailerAddress#1\n");
    }

    if (tag.getSectorTrailerAddress(2) != 11) {
        printf("error in getSectorTrailerAddress#2\n");
    }

    if (tag.getSectorTrailerAddress(3) != 15) {
        printf("error in getSectorTrailerAddress#3\n");
    }

    if (tag.getSectorTrailerAddress(31) != 127) {
        printf("error in getSectorTrailerAddress#5\n");
    }

    if (tag.getSectorTrailerAddress(32) != 143) {
        printf("error in getSectorTrailerAddress#6\n");
    }

    if (tag.getSectorTrailerAddress(39) != 255) {
        printf("error in getSectorTrailerAddress#7\n");
    }

    return 0;
}
