public class A {
    private B b = new B();

    public int execute(int x) {
        int result = b.process(x);
        // 逻辑依赖 B 的结果
        return result * 3;
    }
}
