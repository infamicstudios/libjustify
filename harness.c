#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "cprintf.h"


void
ischartoken_test(){
    const char * const restrict p = "Hello, world!\n";
    const char * q = NULL;
    bool rc;

    q = NULL;
    rc = ischartoken( p, 'H', &q );
    assert( rc == true );
    assert( q == p+1 );

    q = NULL;
    rc = ischartoken( p, 'e', &q );
    assert( rc == false );
    assert( q == p );

    q = NULL;
    rc = ischartoken( p, 'H', NULL );
    assert( rc == true );
    assert( q == NULL );

    q = NULL;
    rc = ischartoken( p, 'e', NULL );
    assert( rc == false );
    assert( q == NULL );
}

void
isstrtoken_test(){
    const char * const restrict p = "Hello, world!\n";
    const char * q = NULL;
    bool rc;

    q = NULL;
    rc = isstrtoken( p+2, "ll", &q );
    assert( rc == true );
    assert( q == p+4 );

    q = NULL;
    rc = isstrtoken( p+2, "qq", &q );
    assert( rc == false );
    assert( q == p+2 );

    q = NULL;
    q = NULL;
    rc = isstrtoken( p+2, "ll", NULL );
    assert( rc == true );
    assert( q == NULL );

    q = NULL;
    rc = isstrtoken( p+2, "qq", NULL );
    assert( rc == false );
    assert( q == NULL );
}

void
create_atom_test(){
    struct atom *a;
    /* 11 21 31
     * 12 22 32
     * 13 23 33
     */
    for( int i = 1; i<4; i++ ){
        for( int j = 10; j<40; j=j+10 ){
            if( j == 10 ){
                a = create_atom( true );
            }else{
                a = create_atom( false );
            }
            a->new_field_width = i+j;
        }
    }
    dump_graph();
    /*
    assert( a->new_field_width );
    assert( 33 == a->new_field_width );
    assert( a->left->new_field_width );
    assert( 23 == a->left->new_field_width );
    assert( a->left->left->new_field_width );
    assert( 13 == a->left->left->new_field_width );
    assert( a->left->left->up );
    assert( a->left->left->up->new_field_width );
    assert( 12 == a->left->left->up->new_field_width );
    assert( 11 == a->left->left->up->up->new_field_width );
    assert( 11 == a->up->up->left->left->new_field_width );
    assert( 11 == a->up->left->up->left->new_field_width );
    */
}

int main(){
    ischartoken_test();
    isstrtoken_test();
    create_atom_test();   
    return 0;

}

