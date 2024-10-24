# HaloDataManager

## Requirements
The code was written with the C++17 standard in mind. I only tested compiling with Linux with **gcc/11.4.0**.

## Installation
The reading/managing library is a header-only library so that you can simply include the **io/DataIO.hpp** file into your C++ source to get all of the functionality.

## License
GPL-3.0 license. Feel free to modify and send back pull requests.

## Testing
There is a small test suite in the **test/** folder. To build and run the entire suite, run the **build_and_run_tests.sh** script. If there are no error messages, then the tests passed!

## Examples
There is a folder **examples/** that contain a few examples of how to use the library. Additionally, the **test/** folder could also be used as a source of examples for certain functionality.

### Reading data
There are two main, important classes that allow one to read data: **DataIO** and **DataContainer**. The **DataIO** class is a template-class that depends on the type of **DataContainer**. Additionally, **DataContainer** is a template that depends on the structures defined in **io/DataContainer.hpp**. 

The simplest usage, to read two columns in a Rockstar halo catalog, might look something like this:

    #include <vector>
    #include <string>
    #include <iostream>
    #include "io/DataIO.hpp"
    int main(int argc, char* argv[]) {
        DataIO<DataContainer<RockstarData>> data_io("file.list");

        // define the columns that you want
        std::vector<std::string> mask = {"id", "virial_mass"};

        // define the data container
        DataContainer<RockstarData> data(mask);

        // fill the container
        data_io.read_data_from_file(data);

        // print the 1st halo's virial mass
        auto mass = data.get_data<double>(0, data.get_internal_key("virial_mass"));
        std::cout << "Mass is " << mass << " Msun/h" << std::endl;

        return 0;
	}
	
### Building merger trees

There is functionality in the software to also construct merger trees. All of that functionality is defined in the **tree/** folder. A single **Tree** object relates to a particular root node, and is built from the top-down, where the "top" is the lowest redshift in the data. In principle, the tree could be built from any starting node.

As a simple example, it is possible to read Consistent-Trees data as above and then construct a tree:

    #include <vector>
    #include <string>
    #include <iostream>
    #include <memory>
    #include "io/DataIO.hpp"
    int main(int argc, char* argv[]) {
        DataIO<DataContainer<ConsistentTreesData>> data_io("file.list");
        // we need these to construct the tree
        std::vector<std::string> mask = {"id", "descendant_id"};

        // define the data container
        DataContainer<ConsistentTreesData> data(mask);

        // fill the container
        size_t N_lines = data_io.read_data_from_file(data);
        size_t id_key = data.get_internal_key("id");
        size_t descendant_id_key = data.get_internal_key("descendant_id");
        size_t first_root_row = 0;
        size_t second_root_row = 0;

        // find the first and second root nodes in the file
        for (size_t row = 0; row < N_lines; row++) {
            if (data.get_data<int64_t>(row, descendant_id_key) == -1) {
                first_root_row = row;

                // start at the next row to look for the next root node
                for (size_t next_row = row + 1; next_row < N_lines; next_row++) {
                    if (data.get_data<int64_t>(next_row, descendant_id_key) == -1) {
                        second_root_row = next_row;
                        break;
                    }
                }
                break;
            }
        }
		
        // we need the id to make the parent node
        // nullptr is the pointer to the parent (none in this case)
        auto id = data.get_data<int64_t>(first_root_row, id_key);
        auto root_node = std::make_shared<Node>(first_root_row, nullptr, id);
		
        // make the tree based on the root node
        auto tree = std::make_shared<Tree>(root_node, first_root_row, second_root_row);

        // build the tree from the loaded data
        tree->build_tree(data);

        return 0;
    }

### Tree traversal

There is a utility function **breadth_first_search** that can search the constructed tree given a data set, starting node, key, query, and condition:

    // start from root node and find all halos greater than
	// 10^9 Msun/h in the past.
	size_t virial_mass_key = data.get_internal_key("virial_mass");
    auto nodes = tree->breadth_first_search(data, tree->root_node_, virial_mass_key, 1.e9, std::greater<double>());

The variable **nodes** will contain a vector of pointers to the nodes within the Tree that satisfy the criteria. More specifically, it returns:

    std::vector<std::shared_ptr<Node>>
