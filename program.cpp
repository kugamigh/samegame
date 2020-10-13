#include "splashkit.h"
#include <vector>

using namespace std;

// Define grid dimensions, for easier manipulation
#define GRID_WIDTH 10
#define GRID_HEIGHT 10

// Define how big the cells in the grid are, for easier manipulation
#define CELL_SIZE 50

/**
 * Each cell has an x position, y position and a colour
 * 
 * @field x_pos         The x position of the cell within the grid
 * @field y_pos         The y position of the cell within the grid
 * @field cell_colour   The colour of the cell
 */
struct cell_data
{
    int                 x_pos;
    int                 y_pos;
    color               cell_colour;
};

/**
 * A grid is made up of cells.  It has height and width, which is
 * the number of cells high and wide it can go.  The cells within
 * the grid have a size.
 * 
 * @field cells         The cells contained within the grid
 * @field height        The number of rows the grid has
 * @field width         The number of columns the grid has
 * @field size          The height and width of the cells within the grid
 */
struct grid_data
{
    vector<cell_data>   cells;
    int                 height;
    int                 width;
    int                 size;
};

// Declaring enum for colour choices
enum available_colours
{
    RED,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE,
    WHITE
};

/**
 * Define a new grid with the given dimensions.  It 
 * has n columns and n rows worth of cells.  Each cell
 * has an x position and y position within the grid.
 * 
 * @param grid      The grid we are shaping
 * @param rows      The number of rows within the grid
 * @param columns   The number of columns within the grid
 */
void initialise_grid(grid_data &grid, int rows, int columns, int size)
{
    cell_data current_cell;

    grid.height = rows;
    grid.width = columns;
    grid.size = size;
    

    // For each row of the grid
    for ( int j = 0; j < columns; j++ )
    {
        // For each column of the grid
        for ( int i = 0; i < rows; i++ )
        {
            // Set the x and y positions for the cell
            current_cell.x_pos = i;
            current_cell.y_pos = j;

            // Give a default colour to the cell
            current_cell.cell_colour = COLOR_WHITE;

            // Put the cell into the grid
            grid.cells.push_back(current_cell);
        }
    }
}

/**
 * Selects a random number and then casts that to one of our
 * available colours
 * 
 * @return selected colour from the available_colours enum
 */
available_colours choose_colour()
{
    return static_cast<available_colours>(rnd(5));
}

/**
 * Provides a random colour to a given cell if it has no colour
 * or changes it to the next colour
 * 
 * @param cell      The cell we want to give colour to
 */
void colour_cell(cell_data &cell)
{
    color selected_colour;
    int colour_index;

    // Pick a random colour
    colour_index = choose_colour();

    // Depending on the colour chosen, recolour our cell
    switch (colour_index)
    {
        case RED:
            selected_colour = COLOR_RED;
            break;
        case BLUE:
            selected_colour = COLOR_BLUE;
            break;
        case GREEN:
            selected_colour = COLOR_GREEN;
            break;
        case YELLOW:
            selected_colour = COLOR_YELLOW;
            break;
        case PURPLE:
            selected_colour = COLOR_PURPLE;
            break;
        default:
            selected_colour = COLOR_WHITE;
            write_line("Colour selection failed");
            break;
    }

    // Apply the colour to the cell
    cell.cell_colour = selected_colour;
}

/**
 * Assigns a colour to each of the cells within the given grid
 * 
 * @param grid      The grid we are providing colours for
 */
void assign_colours(grid_data &grid)
{
    // For each cell in the grid
    for ( int i = 0; i < grid.cells.size(); i++ )
    {
        // Give it a colour
        colour_cell(grid.cells[i]);
    }
}

/**
 * Takes each of the cells and draws them on screen based 
 * on their x and y positions, according to their assigned
 * colour
 * 
 * @param grid      The grid we are drawing, containing all cell data
 * @param size      The height and width of the cells that we are drawing
 */
void draw_grid(grid_data &grid, double size)
{
    // For each cell in the grid
    for ( int i = 0; i < grid.cells.size(); i++ )
    {
        // Draw the cell itself
        fill_rectangle(grid.cells[i].cell_colour,   // Get the colour
            grid.cells[i].x_pos * size,             // Get the x position
            grid.cells[i].y_pos * size,             // Get the y position
            size,                                   // Define width
            size);                                  // Define height

        // Draw the border for each cell
        draw_rectangle(COLOR_BLACK,                 // Black border
            grid.cells[i].x_pos * size,             // Get the x position
            grid.cells[i].y_pos * size,             // Get the y position
            size,                                   // Define width
            size);                                  // Define height
    }
}

/**
 * Checks to see if two cells have the same colour
 * 
 * @param first_cell        First cell which we are comparing
 * @param second_cell       Second cell which we are comparing
 * 
 * @returns True or false whether they match or not
 */
bool same_colour(const cell_data &first_cell, const cell_data &second_cell)
{
    return (color_to_string(first_cell.cell_colour) == color_to_string(second_cell.cell_colour));
}

/**
 * Checks to see if a given cell is white
 * 
 * @param cell              The cell to check the colour of
 * 
 * @returns                 True if the cell is white
 */
bool is_white(const cell_data &cell)
{
    return color_to_string(cell.cell_colour) == color_to_string(COLOR_WHITE);
}

/**
 * Matches cells together if same_colour comes back true by turning them
 * both white.  An original copy is passed through so that all cells can be
 * recoloured at once instead of at the end.
 * 
 * @param grid              The grid which contains the cells we're comparing
 * @param first_cell_index  The first cell's index within grid.cells
 * @param second_cell_index The second cell's index within grid.cells
 * @param original_cell     The original cell which is being compared
 */
void match_cells(grid_data &grid, int first_cell_index, int second_cell_index, const cell_data original_cell)
{
    if ( same_colour(original_cell, grid.cells[second_cell_index]) )
    {
        grid.cells[first_cell_index].cell_colour = COLOR_WHITE;
        grid.cells[second_cell_index].cell_colour = COLOR_WHITE;
    }
}

/**
 * Checks neighbouring cells and then sends them through to match_cells
 * if they are valid indexes (not out of bounds from the grid).
 */
void check_neighbours(grid_data &grid, int current_cell_index)
{
    cell_data current_cell;
    vector<int> cells_indexes_to_compare;
    int cells_remaining;
    
    current_cell = grid.cells[current_cell_index];
    
    // Check if we're on an edge/border cell before adding to the cells we want to check
    // Top border
    if ( current_cell.y_pos != 0 )
    {
        cells_indexes_to_compare.push_back(current_cell_index - grid.width);
    }

    // Right border
    if ( current_cell.x_pos != (grid.width - 1) )
    {      
        cells_indexes_to_compare.push_back(current_cell_index + 1);
    }

    // Bottom border
    if ( current_cell.y_pos != (grid.height - 1) )
    {     
        cells_indexes_to_compare.push_back(current_cell_index + grid.width);
    }

    // Left border
    if ( current_cell.x_pos != 0 )
    {
        cells_indexes_to_compare.push_back(current_cell_index - 1);
    }

    // Match each of the cells
    for ( int i = 0; i < cells_indexes_to_compare.size(); i++ )
    {
        match_cells(grid, 
            current_cell_index, 
            cells_indexes_to_compare[i],
            current_cell);
    }
}

/**
 * Checks to see if the cell below any given cell is empty (coloured white).
 * If it is, swap the y_pos property of each cell until it hits the bottom
 * 
 * @param grid              The grid which we are updating cell positions for
 */
void update_cell_positions(grid_data &grid)
{
    // Check each cell in the grid
    for ( int i = 0; i < grid.cells.size(); i++ )
    {
        // Make sure we're not on the bottom row
        if ( grid.cells[i].y_pos < grid.height - 1 )
        {
            // Check if the cell below the current cell is white and the current cell is not white
            if ( (is_white(grid.cells[i + grid.height])) and
            (!is_white(grid.cells[i])) )
            {
                // Swap the cells' y position and their position within the vector
                swap(grid.cells[i].y_pos, grid.cells[i + grid.width].y_pos);
                swap(grid.cells[i], grid.cells[i + grid.width]);
            }
        }
    }
}

/**
 * Checks to see if there are any valid moves left in the grid
 * 
 * @params grid             The grid we are checking for valid moves
 * 
 * @returns                 True if there are any cells that can still match
 */
bool has_matches(const grid_data &grid)
{
    // Check each cell for matches
    for ( int i = 0; i < grid.cells.size(); i++ )
    {
        // Only check the ones that aren't white
        if ( !is_white(grid.cells[i]) )
        {
            // Check the cell above exists
            if ( (i - grid.width) > 0 )
            {
                // Compare colours
                if ( same_colour(grid.cells[i], grid.cells[i - grid.width]) )
                {
                    return true;
                }
            }

            // Check the cell to the right exists
            if ( (i + 1) < grid.width )
            {
                // Compare colours
                if ( same_colour(grid.cells[i], grid.cells[i + 1]) )
                {
                    return true;
                }
            }

            // Check the cell below exists
            if( (i + grid.width) < grid.cells.size() )
            {
                // Compare colours
                if ( same_colour(grid.cells[i], grid.cells[i + grid.width]) )
                {
                    return true;
                }
            }

            // Check the cell to the left exists
            if ( (i - 1) > 0 ) 
            {
                // Compare colours
                if ( same_colour(grid.cells[i], grid.cells[i - 1]) )
                {
                    return true;
                }
            }
        }
    }
    
    // If there aren't any matches, return false
    return false;
}

/**
 * Takes the position of the mouse and checks to see if it matches
 * a region where a cell is positioned
 * 
 * @return  The index of the cell where the mouse was located, or -1 if none found
 */
int cell_under_mouse(grid_data &grid)
{
    int mouse_x, mouse_y, cell_row, cell_col, cell_index;

    // Capture mouse coordinates as integers
    mouse_x = mouse_position().x;
    mouse_y = mouse_position().y;

    // Check that we have clicked somewhere that contains a cell
    if ( mouse_x < grid.width * grid.size )
    {
        if ( mouse_y < grid.height * grid.size )
        {
            // Define which row and column we're in
            cell_row = mouse_x / (grid.size);
            cell_col = mouse_y / (grid.size);

            // Convert the 2D co-ordinate to an integer
            cell_index = cell_row + (cell_col * grid.width);
            
            // Return the index of the cell
            return cell_index;
        }
        // If out of bounds, return -1
        else
        {
            return -1;
        }
    }
    // If out of bounds, return -1
    else
    {
        return -1;
    }
}

/**
 * Takes all of the keyboard and mouse inputs and then based on what
 * was pressed, call other functions.  For example when clicking 
 * the mouse, check if the mouse is over a cell.
 * 
 * @param grid      The grid of cells which we need the position of
 */
void handle_input(grid_data &grid)
{
    int mouse_x, mouse_y, cell_index;

    // If we click the left mouse button
    if ( mouse_clicked(LEFT_BUTTON) )
    {
        cell_index = cell_under_mouse(grid);

        // If we clicked a cell
        if ( cell_under_mouse(grid) >= 0 )
        {
            // Check neighbours
            check_neighbours(grid, cell_index);
        }
    }

    // If we press R, restart
    if ( key_typed(R_KEY) )
    {
        // Reassign the colours to every cell
        assign_colours(grid);

        write_line("Resetting colours");
    }
}

int main()
{
    grid_data my_grid;

    // Initialise our grid with its width, height and cell size
    initialise_grid(my_grid, GRID_WIDTH, GRID_HEIGHT, CELL_SIZE);

    // Give it some colours
    assign_colours(my_grid);

    // Open a window the size of our grid
    open_window("Same Game", 
        GRID_WIDTH * CELL_SIZE, 
        GRID_HEIGHT * CELL_SIZE);

    while ( not quit_requested() )
    {
        process_events();
        
        handle_input(my_grid);

        draw_grid(my_grid, CELL_SIZE);

        update_cell_positions(my_grid);

        if ( !has_matches(my_grid) )
        {
            write_line("You won!");
            assign_colours(my_grid);
        }

        refresh_screen(60);
    }

    return 0;
}