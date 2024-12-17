#ifndef PUSTAKA_H
#define PUSTAKA_H

#include <sqlite3.h>

#define MAX_STR 200
#define MAX_BUKU 100

// Struktur data buku
typedef struct {
    int id;
    char judul[MAX_STR];
    char penulis[MAX_STR];
    int tahun;
    char kategori[MAX_STR];
    int status_tersedia;
} Buku;

// Fungsi-fungsi untuk operasi database
int buat_tabel(sqlite3 *db);
int tambah_buku(sqlite3 *db, Buku *buku);
int tampilkan_buku(sqlite3 *db);
int cari_buku(sqlite3 *db, const char *judul);
int pinjam_buku(sqlite3 *db, int id_buku);
int kembalikan_buku(sqlite3 *db, int id_buku);

// Fungsi utilitas
void tampilkan_menu();
Buku input_buku();
int validasi_input(Buku *buku);

#endif // PUSTAKA_H