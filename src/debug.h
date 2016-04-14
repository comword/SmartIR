/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#pragma once
#include <iostream>
#include <vector>

/**
 * If you add an entry, add an entry in that function:
 * std::ostream &operator<<(std::ostream &out, DebugLevel lev)
 */
enum DebugLevel {
    D_INFO          = 1,
    D_WARNING       = 1 << 2,
    D_ERROR         = 1 << 3,

    DL_ALL = ( 1 << 5 ) - 1
};
/**
 * Debugging areas can be enabled for each of those areas separately.
 * If you add an entry, add an entry in that function:
 * std::ostream &operator<<(std::ostream &out, DebugClass cl)
 */
enum DebugClass {
    /** Messages from realDebugmsg */
    D_MAIN    = 1,
    D_MODS     = 1 << 2,
    D_NETWORK = 1 << 3,

    D_PYTHON    = 1 << 4,

    DC_ALL    = ( 1 << 30 ) - 1
};
void setupDebug();
void deinitDebug();
// Function Declatations                                            {{{1
// ---------------------------------------------------------------------
/**
 * Set debug levels that should be logged. bitmask is a OR-combined
 * set of DebugLevel values. Use 0 to disable all.
 * Note that D_ERROR is always logged.
 */
void limitDebugLevel( int );
/**
 * Set the debug classes should be logged. bitmask is a OR-combined
 * set of DebugClass values. Use 0 to disable all.
 * Note that D_UNSPECIFIC is always logged.
 */
void limitDebugClass( int );

// Debug Only                                                       {{{1
// ---------------------------------------------------------------------

// See documentation at the top.
void init_log();
void redirect_out_to_log(bool screen);
std::string currentTime();
//std::ofstream &currentLevel();
std::ostream &DebugLog( DebugLevel, DebugClass );
// OStream operators                                                {{{1
// ---------------------------------------------------------------------

template<typename C, typename A>

std::ostream &operator<<( std::ostream &out, const std::vector<C, A> &elm )
{
    bool first = true;
    for( typename std::vector<C>::const_iterator
         it = elm.begin(),
         end = elm.end();
         it != end; ++it ) {
        if( first ) {
            first = false;
        } else {
            out << ",";
        }
        out << *it;
    }

    return out;
}

extern bool debug_mode;
extern bool debug_fatal;
#if !(defined _WIN32 || defined WINDOWS || defined __CYGWIN__)
void * log_thread(void *ptr);
#endif
