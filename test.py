from PIL import Image

i = j = 499

im = Image.open('Images/512/forest_color.jpg')
rgb_im = im.convert('RGB')
r, g, b = rgb_im.getpixel((0, 0))
print(0, 0, r, g, b)
r, g, b = rgb_im.getpixel((0, 1))
print(0, 1, r, g, b)
r, g, b = rgb_im.getpixel((0, 2))
print(0, 2, r, g, b)
r, g, b = rgb_im.getpixel((1, 0))
print(1, 0, r, g, b)
r, g, b = rgb_im.getpixel((2, 0))
print(2, 0, r, g, b)
r, g, b = rgb_im.getpixel((40, 40))
print(40, 40, r, g, b)
r, g, b = rgb_im.getpixel((499, 499))
print(499, 499, r, g, b)


