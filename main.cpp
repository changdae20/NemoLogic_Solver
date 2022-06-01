#include <algorithm>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

enum class CELL {
    FILL = 1,
    BLANK = 0,
    UNKNOWN = -1
};

void print( std::vector<int> &arr ) {
    for ( auto &el : arr ) {
        std::cout << el << ( ( &el ) != ( &arr.back() ) ? ", " : "" );
    }
    std::cout << "\n";
}

void print( std::vector<std::vector<int>> &arr ) {
    for ( auto &row : arr ) {
        print( row );
    }
}

unsigned long long combination( unsigned long long n, unsigned long long k ) {
    if ( k > n )
        return 0uLL;
    if ( k * 2 > n )
        k = n - k;
    if ( k == 0 )
        return 1uLL;

    unsigned long long result = n;
    for ( unsigned long long i = 2; i <= k; ++i ) {
        result *= ( n - i + 1 );
        result /= i;
    }
    return result;
}

std::vector<std::vector<int>> make_candidates( const std::vector<int> &hint, int board_size ) {
    std::vector<std::vector<int>> candidates;
    auto sum = std::accumulate( hint.begin(), hint.end(), 0 );

    candidates.reserve( combination( hint.size() + 1, board_size - sum - hint.size() + 1 ) );
    //std::cout << "Line " << __LINE__ << " : board_size : " << board_size  << std::endl;
    //std::cout << "Line " << __LINE__ << " : sum : " << sum << std::endl;
    std::vector<bool> temp( board_size - sum + 1, true );
    // std::cout << __LINE__ << ", " << temp.size() << std::endl;
    for ( size_t i = 0; i < hint.size(); ++i )
        temp[ i ] = false; // temp = [(false) * hint.size() , (true) * (board_size - hint.size() + 1)], total size of board_size.

    do {
        size_t hint_index = 0;
        std::vector<int> candidate;
        candidate.reserve( board_size );
        for ( const auto &el : temp ) {
            if ( !el ) {
                for ( int i = 0; i < hint.at( hint_index ); ++i )
                    candidate.push_back( static_cast<int>( CELL::FILL ) );
                if ( hint_index < hint.size() - 1 )
                    candidate.push_back( static_cast<int>( CELL::BLANK ) );
                ++hint_index;
            } else {
                candidate.push_back( static_cast<int>( CELL::BLANK ) );
            }
        }
        candidates.push_back( candidate );
    } while ( std::next_permutation( temp.begin(), temp.end() ) );

    return candidates;
}

bool is_done( const std::vector<std::vector<int>> &board ) {
    return std::all_of( board.begin(), board.end(), []( const auto &line ) {
        return std::all_of( line.begin(), line.end(), []( const auto &el ) {
            return el != static_cast<int>( CELL::UNKNOWN );
        } );
    } );
}

bool perform_row( std::vector<std::vector<int>> &board, std::vector<std::vector<int>> &row_candidates, int row ) {
    bool changed = false;

    // 현재 보드 상태에 맞춰서 row candidates 쳐낼거 쳐냄
    for ( size_t i = 0; i < board[ row ].size(); ++i ) {
        for ( auto iter = row_candidates.begin(); iter != row_candidates.end(); ) {
            if ( board[ row ][ i ] == static_cast<int>( CELL::UNKNOWN ) )
                ++iter;
            else if ( board[ row ][ i ] != ( *iter )[ i ] ) {
                iter = row_candidates.erase( iter );
                changed = true;
            } else
                ++iter;
        }
    }

    // row candidates 순회하면서 겹치는거 있는지?
    for ( size_t i = 0; i < board[ row ].size(); ++i ) {
        if ( board[ row ][ i ] != static_cast<int>( CELL::UNKNOWN ) )
            continue;
        // 겹치는거 있으면 보드상태 업데이트
        if ( std::all_of( row_candidates.begin(), row_candidates.end(), [ &row_candidates, &row, &i ]( const auto &candidate ) { return row_candidates[ 0 ][ i ] == candidate[ i ]; } ) ) {
            board[ row ][ i ] = row_candidates[ 0 ][ i ];
            changed = true;
        }
    }

    return changed;
}

bool perform_col( std::vector<std::vector<int>> &board, std::vector<std::vector<int>> &col_candidates, int col ) {
    bool changed = false;

    // 현재 보드 상태에 맞춰서 col candidates 쳐낼거 쳐냄
    for ( size_t i = 0; i < board.size(); ++i ) {
        for ( auto iter = col_candidates.begin(); iter != col_candidates.end(); ) {
            if ( board[ i ][ col ] == static_cast<int>( CELL::UNKNOWN ) )
                ++iter;
            else if ( board[ i ][ col ] != ( *iter )[ i ] ) {
                iter = col_candidates.erase( iter );
                changed = true;
            } else
                ++iter;
        }
    }

    // col candidates 순회하면서 겹치는거 있는지?
    for ( size_t i = 0; i < board.size(); ++i ) {
        if ( board[ i ][ col ] != static_cast<int>( CELL::UNKNOWN ) )
            continue;
        // 겹치는거 있으면 보드상태 업데이트
        if ( std::all_of( col_candidates.begin(), col_candidates.end(), [ &col_candidates, &col, &i ]( const auto &candidate ) { return col_candidates[ 0 ][ i ] == candidate[ i ]; } ) ) {
            //std::cout << "Line " << __LINE__ << " : all of same! (row,col) : " << "(" << i << ", " << col << ") = " << col_candidates[0][i] << std::endl;
            board[ i ][ col ] = col_candidates[ 0 ][ i ];
            changed = true;
        }
    }
    return changed;
}

void print_board( std::vector<std::vector<int>> &board ) {
    for ( const auto &row : board ) {
        for ( const auto &el : row ) {
            if ( el == static_cast<int>( CELL::FILL ) )
                std::cout << "O";
            else if ( el == static_cast<int>( CELL::BLANK ) )
                std::cout << " ";
            else if ( el == static_cast<int>( CELL::UNKNOWN ) )
                std::cout << "?";
        }
        std::cout << std::endl;
    }
}

std::vector<std::string> split( std::string input, char delimiter ) {
    std::vector<std::string> answer;
    std::stringstream ss( input );
    std::string temp;

    while ( std::getline( ss, temp, delimiter ) ) {
        answer.push_back( temp );
    }

    return answer;
}

std::vector<std::vector<int>> make_board( std::string filename, std::vector<std::vector<int>> &row_hints, std::vector<std::vector<int>> &col_hints ) {
    std::vector<std::vector<int>> board;

    std::ifstream file( filename );
    if ( file.is_open() ) {
        std::string line;
        getline( file, line );
        auto row_col = split( line, ' ' );
        int row = std::stoi( row_col[ 0 ] );
        int col = std::stoi( row_col[ 1 ] );

        // std::cout << "line " << __LINE__ << " : make board with (row, col) = (" << row << ", " << col << ")" <<std::endl;
        board = std::vector<std::vector<int>>( row, std::vector<int>( col, static_cast<int>( CELL::UNKNOWN ) ) );

        // row hints 읽어서 등록
        while ( row-- ) {
            getline( file, line );
            auto hint = split( line, ' ' );
            std::vector<int> temp;
            std::for_each( hint.begin(), hint.end(), [ &temp ]( const auto &el ) { temp.push_back( std::stoi( el ) ); } );
            row_hints.push_back( temp );
        }

        // col hints 읽어서 등록
        while ( col-- ) {
            getline( file, line );
            auto hint = split( line, ' ' );
            std::vector<int> temp;
            std::for_each( hint.begin(), hint.end(), [ &temp ]( const auto &el ) { temp.push_back( std::stoi( el ) ); } );
            col_hints.push_back( temp );
        }
    }

    return board;
}

int main( int argc, char *argv[] ) {
    clock_t start, end;
    start = clock();
    // 예제 세팅
    std::vector<std::vector<int>> row_hints, col_hints;
    auto board = make_board( "board.txt", row_hints, col_hints );
    // std::cout << "======= row_hints =======" << std::endl;
    // print(row_hints);
    // std::cout << "======= col_hints =======" << std::endl;
    // print(col_hints);
    // std::cout << "======= board =======" << std::endl;
    // print_board(board);

    // 행, 열별 가능한 경우의 수들 미리 계산
    std::vector<std::pair<std::vector<std::vector<int>>, std::pair<int, int>>> row_candidates, col_candidates;

    size_t idx = 0;
    std::for_each( row_hints.begin(), row_hints.end(), [ &row_candidates, &board, &idx ]( const auto &hint ) {
        auto temp = make_candidates( hint, board[ 0 ].size() );
        row_candidates.push_back( std::pair<std::vector<std::vector<int>>, std::pair<int, int>>( temp, std::make_pair<int, int>( idx++, temp.size() ) ) );
        // std::cout << "row hint generated with size " << temp.size() << "\n";
    } );
    std::sort( row_candidates.begin(), row_candidates.end(), []( const auto &a, const auto &b ) { return a.second.second < b.second.second; } );

    idx = 0;
    std::for_each( col_hints.begin(), col_hints.end(), [ &col_candidates, &board, &idx ]( const auto &hint ) {
        auto temp = make_candidates( hint, board.size() );
        col_candidates.push_back( std::pair<std::vector<std::vector<int>>, std::pair<int, int>>( temp, std::make_pair<int, int>( idx++, temp.size() ) ) );
        // std::cout << "col hint generated with size " << temp.size() << "\n";
    } );
    std::sort( col_candidates.begin(), col_candidates.end(), []( const auto &a, const auto &b ) { return a.second.second < b.second.second; } );

    for ( size_t i = 0; i < 10000; ++i ) {
        size_t j = 0;
        while ( !perform_row( board, row_candidates[ j ].first, row_candidates[ j ].second.first ) ) {
            j++;
            if ( j >= board.size() )
                break;
        }
        if ( j < board.size() )
            row_candidates[ j ].second.second = row_candidates[ j ].first.size();
        std::sort( row_candidates.begin(), row_candidates.end(), []( const auto &a, const auto &b ) { return a.second.second < b.second.second; } );

        j = 0;
        while ( !perform_col( board, col_candidates[ j ].first, col_candidates[ j ].second.first ) ) {
            j++;
            if ( j >= board[ 0 ].size() )
                break;
        }
        if ( j < board.size() )
            col_candidates[ j ].second.second = col_candidates[ j ].first.size();
        std::sort( col_candidates.begin(), col_candidates.end(), []( const auto &a, const auto &b ) { return a.second.second < b.second.second; } );

#ifdef _WIN32
        system( "cls" );
#endif
#ifdef linux
        system( "clear" );
#endif

        std::cout << "===== " << i + 1 << "th iteration =====" << std::endl;
        print_board( board );
        if ( is_done( board ) )
            break;
    }
#ifdef _WIN32
    system( "cls" );
#endif
#ifdef linux
    system( "clear" );
#endif
    std::cout << "===== Result =====" << std::endl;
    print_board( board );
    if ( is_done( board ) )
        std::cout << "DONE!!" << std::endl;
    else
        std::cout << "NOT DONE..." << std::endl;

    end = clock();

    std::cout << std::fixed;
    std::cout << "Elapsed Time : " << static_cast<double>( end - start ) / CLOCKS_PER_SEC << "s" << std::endl;
    return 0;
}
