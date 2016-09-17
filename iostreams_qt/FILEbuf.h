// see http://web.archive.org/web/20030202124616/www.cmla.ens-cachan.fr/Utilisateurs/dosreis/C++/FAQ/

#include <stdio.h>
#include <assert.h>

#include <iostream>
#include <streambuf>

// streambuf minimal encapsulant un FILE*
//    - utilise les tampons de FILE donc n'a pas de tampon interne en
//      sortie et a un tampon interne de taille 1 en entrée car l'interface
//      de std::streambuf ne permet pas de faire moins;
//    - ne permet pas la mise en place d'un tampon
//    - une version plus complète devrait permettre d'accéder aux
//      informations d'erreur plus précises de FILE* et interfacer aussi
//      les autres possibilités de FILE* (entre autres synchroniser les
//      sungetc/sputbackc avec la possibilité correspondante de FILE*)

class FILEbuf: public std::streambuf
{
public:

    explicit FILEbuf(FILE* cstream);
    // cstream doit être non NULL.

private:

    std::streambuf* setbuf(char_type* s, std::streamsize n);

    int_type overflow(int_type c);
    int      sync();

    int_type underflow();

    FILE*    cstream_;
    char     inputBuffer_[1];
};

inline FILEbuf::FILEbuf(FILE* cstream)
    : cstream_(cstream)
{
    // le constructeur de streambuf équivaut a
    // setp(0, 0);
    // setg(0, 0);
    assert(cstream != NULL);
}

inline std::streambuf* FILEbuf::setbuf(char_type*, std::streamsize)
{
    // ne fait rien, ce qui est autorisé.  Une version plus complète
    // devrait vraisemblablement utiliser setvbuf
    return NULL;
}

inline FILEbuf::int_type FILEbuf::overflow(int_type c)
{
    if (traits_type::eq_int_type(c, traits_type::eof())) {
        // la norme ne le demande pas exactement, mais si on nous passe eof
        // la coutume est de faire la même chose que sync()
        return (sync() == 0
                ? traits_type::not_eof(c)
                : traits_type::eof());
    } else {
        return ((fputc(c, cstream_) != EOF)
                ? traits_type::not_eof(c)
                : traits_type::eof());
    }
}

inline int FILEbuf::sync()
{
    return (fflush(cstream_) == 0 ? 0 : -1);
}


inline FILEbuf::int_type FILEbuf::underflow()
{
    // Assurance contre des implémentations pas strictement conformes à la
    // norme qui garantit que le test est vrai.  Cette garantie n'existait
    // pas dans les IOStream classiques.
    if (gptr() == NULL || gptr() >= egptr()) {
        int gotted = fgetc(cstream_);
        if (gotted == EOF) {
            return traits_type::eof();
        } else {
            *inputBuffer_ = gotted;
            setg(inputBuffer_, inputBuffer_, inputBuffer_+1);
            return traits_type::to_int_type(*inputBuffer_);
        }
    } else {
        return traits_type::to_int_type(*inputBuffer_);
    }
}

// ostream minimal facilitant l'utilisation d'un FILEbuf
// dérive de manière privée de FILEbuf, ce qui permet de s'assurer
// qu'il est bien initialisé avant std::ostream

class oFILEstream: private FILEbuf, public std::ostream
{
public:
    explicit oFILEstream(FILE* cstream);
    // cstream doit être non NULL
};

inline oFILEstream::oFILEstream(FILE* cstream)
    : FILEbuf(cstream), std::ostream(this)
{
}

// istream minimal facilitant l'utilisation d'un FILEbuf
// dérive de manière privée de FILEbuf, ce qui permet de s'assurer
// qu'il est bien initialisé avant std::istream

class iFILEstream: private FILEbuf, public std::istream
{
public:
    explicit iFILEstream(FILE* cstream);
    // cstream doit être non NULL
};

inline iFILEstream::iFILEstream(FILE* cstream)
    : FILEbuf(cstream), std::istream(this)
{
}
