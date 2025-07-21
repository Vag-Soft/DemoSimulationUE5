import open3d as o3d
import matplotlib.pyplot as plt
import cv2

"""
skeletons: (M, N, 3) numpy array
"""
def open3d_multiple_skeletons(skeletons, title="3D Skeletons"):
    vis = o3d.visualization.Visualizer()
    vis.create_window(window_name=title, width=500, height=400)

    for skeleton in skeletons:
        points = o3d.geometry.PointCloud()
        points.points = o3d.utility.Vector3dVector(skeleton)
        vis.add_geometry(points)

    vis.run()
    vis.destroy_window()

'''
skeleton: (N, 3) numpy array
'''
def open3d_single_skeleton(skeleton, title="3D Skeleton"):
    points = o3d.geometry.PointCloud()
    points.points = o3d.utility.Vector3dVector(skeleton)

    vis = o3d.visualization.Visualizer()
    vis.create_window(window_name=title, width=500, height=400)
    vis.add_geometry(points)

    vis.run()
    vis.destroy_window()


'''
skeletons:(M, N, 2) numpy array
'''
def plot2d_multiple_skeletons(skeletons, title="2D Skeletons"):
    plt.figure(figsize=(8, 8))
    for skeleton in skeletons:
        plt.scatter(skeleton[:, 0], skeleton[:, 1], color='b')
    plt.title(title)
    plt.xlabel('X-axis')
    plt.ylabel('Y-axis')
    plt.axis('equal')
    plt.grid()
    plt.gca().invert_yaxis()
    plt.show()


'''
skeleton: (N, 2) numpy array
'''
def plot2d_single_skeleton(skeleton, title="2D Skeleton"):
    plt.figure(figsize=(8, 8))
    plt.scatter(skeleton[:, 0], skeleton[:, 1], color='b')
    plt.title(title)
    plt.xlabel('X-axis')
    plt.ylabel('Y-axis')
    plt.axis('equal')
    plt.grid()
    plt.gca().invert_yaxis()
    plt.show()


def overlay_multiple_skeleton_joints(image, skeletons, radius=1, joint_color=(0, 255, 0), thickness=2):
    img_with_skeletons = image.copy()
    img_height, img_width = img_with_skeletons.shape[:2]

    for skeleton in skeletons:
        for kp_idx in range(skeleton.shape[0]):
            x, y = skeleton[kp_idx]

            if 0 <= x <= 1 and 0 <= y <= 1:
                x_px, y_px = int(x * img_width), int(y * img_height)
            else:
                x_px, y_px = int(x), int(y)

            cv2.circle(img_with_skeletons, (x_px, y_px), radius, joint_color, thickness)

    plt.figure(figsize=(5, 5))
    plt.imshow(cv2.cvtColor(img_with_skeletons, cv2.COLOR_BGR2RGB))
    plt.axis('off')
    plt.subplots_adjust(top=1, bottom=0, right=1, left=0, hspace=0, wspace=0)
    plt.margins(0, 0)
    plt.tight_layout(pad=0)
    plt.show()

    return img_with_skeletons

'''
skeleton: (N, 2) numpy array
image: (H, W, 3) numpy array
'''
def overlay_skeleton_joints(image, skeleton, radius=1, joint_color=(0, 255, 0), thickness=2):
    img_with_skeleton = image.copy()
    img_height, img_width = img_with_skeleton.shape[:2]

    for kp_idx in range(skeleton.shape[0]):
        x, y = skeleton[kp_idx]

        if 0 <= x <= 1 and 0 <= y <= 1:
            x_px, y_px = int(x * img_width), int(y * img_height)
        else:
            x_px, y_px = int(x), int(y)

        cv2.circle(img_with_skeleton, (x_px, y_px), radius, joint_color, thickness)

    plt.figure(figsize=(5, 5))
    plt.imshow(cv2.cvtColor(img_with_skeleton, cv2.COLOR_BGR2RGB))
    plt.axis('off')
    plt.subplots_adjust(top=1, bottom=0, right=1, left=0, hspace=0, wspace=0)
    plt.margins(0, 0)
    plt.tight_layout(pad=0)
    plt.show()

    return img_with_skeleton


def update_overlay_multiple_skeleton_joints(image, skeletons, radius=1, joint_color=(0, 255, 0), thickness=2,
                                     fig=None, ax=None, show=True):
    img_with_skeletons = image.copy()
    img_height, img_width = img_with_skeletons.shape[:2]

    for skeleton in skeletons:
        for kp_idx in range(skeleton.shape[0]):
            x, y = skeleton[kp_idx]

            if 0 <= x <= 1 and 0 <= y <= 1:
                x_px, y_px = int(x * img_width), int(y * img_height)
            else:
                x_px, y_px = int(x), int(y)

            cv2.circle(img_with_skeletons, (x_px, y_px), radius, joint_color, thickness)

    # Create or reuse figure and axis
    if fig is None or ax is None:
        fig, ax = plt.subplots(figsize=(5, 5))

    ax.clear()  # Clear previous content
    ax.imshow(cv2.cvtColor(img_with_skeletons, cv2.COLOR_BGR2RGB))
    ax.axis('off')

    plt.subplots_adjust(top=1, bottom=0, right=1, left=0, hspace=0, wspace=0)
    plt.margins(0, 0)
    plt.tight_layout(pad=0)

    if show:
        plt.draw()  # Update the figure
        plt.pause(0.001)  # Small pause to allow GUI events to process

    return img_with_skeletons, fig, ax

def show_image_matplotlib_and_opencv(image):
    plt.imshow(image)

    cv2.imshow("Image", image)