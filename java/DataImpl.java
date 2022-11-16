import java.util.List;

public class DataImpl implements Data {
    private final int dataId;
    private final List<Integer> vector;

    public DataImpl(int dataId, List<Integer> vector) {
        this.dataId = dataId;
        this.vector = vector;
    }

    public List<Integer> getVector() {
        return vector;
    }

    @Override
    public int getDataId() {
        return dataId;
    }

    @Override
    public int getSize() {
        return vector.size();
    }

    @Override
    public int getValue(int idx) {
        return vector.get(idx);
    }
}
