import { prisma } from "../../lib/prisma";
const Posts = async () => {
  const posts = await prisma.post.findMany();
  return posts.map((post) => (
    <div key={post.title}>
      <h4 className="text-2xl">{post.title}</h4>
      <p>{post.content}</p>
    </div>
  ));
};

export default function Blog() {
  return (
    <div className="flex flex-col gap-4">
      <h1 className="text-3xl">Dev Log</h1>
      <Posts />
    </div>
  );
}
