#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "pustaka.h"

int main() {
    sqlite3 *db;
    int rc = sqlite3_open("pustaka.db", &db);
    
    if (rc) {
        fprintf(stderr, "Tidak dapat membuka database: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    // Buat tabel
    buat_tabel(db);
    
    int pilihan, id_buku;
    Buku buku;

    // Pengulangan menu
    do {
        // Seleksi kondisi menu
        tampilkan_menu();
        scanf("%d", &pilihan);

        switch (pilihan) {
            case 1:
                buku = input_buku();
                tambah_buku(db, &buku);
                break;

            case 2:
                tampilkan_buku(db);
                break;

            case 3:
                printf("Masukkan judul buku: ");
                scanf(" %[^\n]s", buku.judul);
                cari_buku(db, buku.judul);
                break;

            case 4:
                printf("Masukkan ID buku untuk dipinjam: ");
                scanf("%d", &id_buku);
                pinjam_buku(db, id_buku);
                break;

            case 5:
                printf("Masukkan ID buku untuk dikembalikan: ");
                scanf("%d", &id_buku);
                kembalikan_buku(db, id_buku);
                break;

            case 6:
                printf("Terima kasih!\n");
                break;

            default:
                printf("Pilihan tidak valid!\n");
        }
    } while (pilihan != 6);

    sqlite3_close(db);
    return 0;
}