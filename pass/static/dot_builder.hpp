#include <fstream>
#include <string>
#include <cstdint>

enum class EdgeType {
    NodeToNode = 1,
    ClusterToNode = 2,
    NodeToCluster = 3,
    ClusterToCluster = 4,
};

class DotBuilder {
public:
    DotBuilder() {
    }

    explicit DotBuilder(const std::string& file_name)
        : file_name_(file_name)
        , dot_file_(file_name_, std::ios::app)
        , tabs_num_(0) {
    }

    DotBuilder(const DotBuilder& other) = delete;
    DotBuilder& operator=(const DotBuilder& other) = delete;

    ~DotBuilder() {
        if (dot_file_.is_open()) {
             dot_file_.close();
        }
    }

    void SetFile(const std::string& file_name) {
        file_name_ = file_name;
        if (dot_file_.is_open()) {
            dot_file_.close();
        }
        dot_file_.open(file_name_, std::ios::app);
    }

    bool BeginGraph(const std::string& graph_name) {
        if (!dot_file_.is_open()) {
            return false;
        }
        PrintFormatTabs(); dot_file_ << "digraph " << graph_name << " {" << std::endl;
        tabs_num_++;
        PrintFormatTabs(); dot_file_ << "compound=true" << std::endl;
        return true;
    }

    bool EndGraph() {
        if (!dot_file_.is_open()) {
            return false;
        }
        tabs_num_--;
        PrintFormatTabs(); dot_file_ << "}" << std::endl;
        return true;
    }

    bool BeginSubgraph(const std::string& subgraph_name) {
        if (!dot_file_.is_open()) {
            return false;
        }
        PrintFormatTabs(); dot_file_ << "subgraph cluster_" << subgraph_name << " {" << std::endl;
        tabs_num_++;
        CreateNode(subgraph_name);
        AddLabel("style = invis");
        return true;
    }

    bool EndSubgraph() {
        return EndGraph();
    }

    bool CreateNode(const std::string& node_name) {
        if (!dot_file_.is_open()) {
            return false;
        }
        PrintFormatTabs(); dot_file_ << "node_" << node_name << std::endl;
        return true;
    }

    bool AddLabel(const std::string& label) {
        if (!dot_file_.is_open()) {
            return false;
        }
        PrintFormatTabs(); dot_file_ << "[" << label << "]" << std::endl;
        return true;
    }

    bool CreateEdge(const std::string& from, const std::string& to, EdgeType type) {
        if (!dot_file_.is_open()) {
            return false;
        }

        switch (type) {
            case EdgeType::NodeToNode: {
                PrintFormatTabs(); dot_file_ << "node_" << from << "->" << "node_" << to << std::endl;
                break;
            }

            case EdgeType::ClusterToNode: {
                PrintFormatTabs(); dot_file_ << "cluster_" << from << "->" << "node_" << to << std::endl;
                AddLabel("ltail=" + from);
                break;
            }

            case EdgeType::NodeToCluster: {
                PrintFormatTabs(); dot_file_ << "node_" << from << "->" << "cluster_" << to << std::endl;
                AddLabel("lhead=" + to);
                break;
            }

            case EdgeType::ClusterToCluster: {
                PrintFormatTabs(); dot_file_ << "cluster_" << from << "->" << "cluster_" << to << std::endl;
                AddLabel("ltail=" + from);
                AddLabel("lhead=" + to);
                break;
            }

            default: {
                return false;
            }
        }
        return true;
    }

private:
    inline void PrintFormatTabs() {
        for (int16_t i = 0; i < tabs_num_; i++) {
            dot_file_ << "\t";
        }
    }

private:
    std::string file_name_;
    std::ofstream dot_file_;
    int16_t tabs_num_{0};
};
